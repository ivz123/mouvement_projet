
#include <stdlib.h>
#include <string.h>
#include <MQTTClient.h>
#include <unistd.h>
#include <time.h>
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

static int64_t sampling_freq = EI_CLASSIFIER_FREQUENCY; // in Hz.
static int64_t time_between_samples_us = (1000000 / (sampling_freq - 1));
static MQTTClient client; // Déclaration globale du client MQTT

// to classify 1 frame of data you need EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE values
static float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
void sendMQTTMessage(const char* topic, const char* message);
float readMQTTMessage(const char* topic);
void delayMicroseconds(unsigned long delay_us);
void closeMQTTConnection();
void initializeMQTTConnection();


int main()
{
    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t_start);



    while (1) {
		initializeMQTTConnection();
        // fill the features array
        for (size_t ix = 0; ix < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; ix += EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME) {
            int64_t next_tick = time_between_samples_us;
            

            // copy accelerometer data into the features array
            features[ix + 0] = (float)readMQTTMessage("X/");
            features[ix + 1] = (float)readMQTTMessage("Y/");
            features[ix + 2] = (float)readMQTTMessage("Z/");

            delayMicroseconds(next_tick);
        }

        // frame full? then classify
        ei_impulse_result_t result = { 0 };

        // create signal from features frame
        signal_t signal;
        numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);

        // run classifier
        EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
        ei_printf("run_classifier returned: %d\n", res);
        if (res != 0) return 1;

        // print predictions
        ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
            result.timing.dsp, result.timing.classification, result.timing.anomaly);

        // print the predictions
		const char* max_predict = "";
		float max_classification_value = 0.0;
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            ei_printf("%s:\t%.5f\n", result.classification[ix].label, result.classification[ix].value);
			if (result.classification[ix].value > max_classification_value) { 
			max_classification_value = result.classification[ix].value;
			max_predict = ei_classifier_inferencing_categories[ix];
		}
        }
		ei_printf("Le cobaye est entrain de  : %s\n", max_predict);
		sendMQTTMessage("predict_jeston/", max_predict);
    #if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf("anomaly:\t%.3f\n", result.anomaly);
    #endif
    }
	return 0;
}

void initializeMQTTConnection() {
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    MQTTClient_create(&client, "mqtt://172.20.10.2:1883/", "",
                      MQTTCLIENT_PERSISTENCE_NONE, NULL);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if (MQTTClient_connect(client, &conn_opts) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect to the broker.\n");
        exit(EXIT_FAILURE); // Arrêter le programme en cas d'échec de connexion
    } else {
        printf("Success to connect to the broker.\n");
    }
}

// Fonction de fermeture de la connexion MQTT
void closeMQTTConnection() {
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
}

// Fonction pour envoyer un message MQTT
void sendMQTTMessage(const char* topic, const char* message) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    pubmsg.payload = (void*)message;
    pubmsg.payloadlen = (int)strlen(message);
    pubmsg.qos = 1;
    pubmsg.retained = 0;

    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
}

// Fonction pour lire un message MQTT
float readMQTTMessage(const char* topic) {
    MQTTClient_message* message = NULL;
    int rc;

    MQTTClient_subscribe(client, topic, 1);

    rc = MQTTClient_receive(client, (char**)&topic, NULL, &message, 10000);
    if (rc != MQTTCLIENT_SUCCESS || message == NULL) {
        printf("Failed to receive message for topic %s\n", topic);
        return 0.0;
    }

    float value = atof((char*)message->payload);
    MQTTClient_freeMessage(&message);
    return value;
}



void delayMicroseconds(unsigned long delay_us) {
    struct timespec ts;
    ts.tv_sec = delay_us / 1000000;
    ts.tv_nsec = (delay_us % 1000000) * 1000;
    nanosleep(&ts, NULL);
}