#include <stdio.h>

#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

// Callback function declaration
static int get_signal_data(size_t offset, size_t length, float *out_ptr);

// Raw features copied from test sample (Edge Impulse > Model testing)
static float input_buf[] = {
	1.4099, 6.2803, -1.0461, 1.3798, 6.8390, -1.3762, 1.2338, 7.0205, -1.4968, 1.3063, 6.8202, -1.5187, 1.6782, 7.0823, -1.6704, 1.0086, 7.3210, -1.3047, 0.8710, 6.8697, -1.3445, 0.2158, 5.6847, -1.2634, 0.3980, 5.1601, -1.8217, 0.8076, 5.5716, -2.3774, 2.1284, 6.4067, -3.1678, 3.7873, 7.7304, -3.5132, 4.6946, 8.2329, -3.8804, 4.7634, 9.8173, -6.8663, 6.0365, 12.5649, -7.4855, 6.9246, 13.5003, -6.1682, 5.6114, 12.6106, -5.1640, 4.1056, 10.2512, -6.8164, 3.8963, 9.3543, -7.6682, 4.7703, 7.9045, -6.1905, 3.0682, 9.0441, -2.4021, 1.4973, 10.9271, -0.3962, 0.2849, 17.1522, 3.6621, -3.4430, 15.0022, 1.7789, -4.7610, 9.3368, -2.0988, 0.4019, 5.9283, 2.7174, 2.0864, 6.4358, 2.3363, 1.2858, 8.0851, 2.0429, 0.5863, 10.0955, 2.1615, 1.5829, 10.4594, 3.3251, 1.8811, 10.0540, 3.2804, 1.8390, 9.8462, -0.0654, 2.3655, 8.8579, 0.0709, 2.5180, 8.4602, 0.9092, 1.8114, 8.7472, 1.0789, 1.1987, 8.1992, 1.2312, 1.2149, 7.7970, 1.4234, 2.0424, 7.8869, 1.1763, 2.2531, 7.9592, 1.1726, 2.2221, 8.0803, 0.9468, 2.5036, 8.4666, 1.1286, 2.7459, 8.8271, 0.8755, 2.8205, 9.1098, 0.4169, 2.7988, 9.3678, 0.6457, 2.6764, 9.4330, 0.5893, 2.2713, 9.6697, 0.7823, 1.6891, 10.2538, 1.0722, 1.4445, 10.6223, 1.3482, 1.9209, 10.9029, 1.7442, 3.3900, 10.4178, 1.9864, 3.9347, 9.8414, 1.3607, 4.8022, 10.1508, 0.5685, 5.0616, 10.6142, 0.2740, 4.7411, 10.7284, 0.4360, 3.6779, 9.6540, 1.4428, 2.5187, 8.5961, 2.2766, 1.3268, 9.0748, 2.2107, -0.3713, 11.6268, -0.1053, -2.3396, 14.0366, -0.1152, -2.1210, 13.8083, 0.8621, -0.5703, 12.8970, 1.4193, 1.4319, 14.0154, -0.8695, 2.5841, 14.0288, -1.7826, 4.2594, 12.8563, -3.2654, 3.3434, 11.0673, -1.8525, 2.5007, 10.1579, -1.5844, 1.3050, 8.1811, -0.4149, 0.7350, 7.9441, -0.0012, -0.0705, 7.3339, -0.0560, -0.3864, 6.7267, -1.2405, -0.1483, 6.4500, -1.3850, 0.2222, 6.3853, -0.7729, 0.4344, 6.0469, -0.8591, -0.2773, 6.1733, -0.8484, -0.5695, 4.7703, -1.0344, -0.5718, 4.3830, -1.3384, 0.3048, 5.2195, -2.1913, 1.7535, 6.8230, -3.0448, 2.4211, 7.7692, -3.3326, 4.3048, 9.3368, -4.7863, 5.3445, 11.8763, -7.7440, 6.4117, 13.8623, -7.6655, 6.8175, 14.9615, -5.1700, 3.8291, 12.7811, -6.0220, 2.9000, 10.9289, -7.3679, 3.5704, 8.0758, -7.3879, 4.6681, 7.1437, -2.7244, 2.9387, 7.4380, -0.5725, -1.0944, 11.3997, 1.2571, -3.0266, 18.9610, 0.9707, -4.1427, 19.1449, -0.9936, -1.4877, 8.2247, 2.4162, 2.5392, 5.9430, 2.4954, 1.7113, 6.1441, 1.8151, 1.2914, 7.8917, 1.9625, 0.6048, 9.7993, 1.7943, 0.9312, 10.2204, 2.5094, 1.6608, 10.5035, 1.9439, 1.5643, 10.5604, 0.3150, 1.8613, 9.2557, -0.2179, 1.8616, 8.4035, 1.2671, 1.6517, 8.1699, 1.0677, 0.8456, 8.2769, 0.4688, 1.6074, 8.4213, 0.8633, 2.2663, 8.6265, 0.8859, 2.7430, 8.8083, 0.7066, 2.5506, 8.7930, 0.6515, 2.4717, 8.7156, 0.7926, 2.1760, 8.2028, 0.4138, 2.6499, 8.6161, 0.4883, 2.8062, 9.1948, 0.4646, 2.5416, 9.3319, 0.7917, 2.2606, 9.6280, 1.0931, 1.8166, 9.6199, 1.2478, 1.2588, 10.7200, 1.4093, 1.4257, 11.6295, 1.4491, 1.5704, 10.9306, 1.5836, 2.3481, 10.1522, 1.5368, 3.5524, 10.4368, 0.6562, 3.6796, 10.1442, 0.5207, 4.9835, 11.0292, 0.4034, 3.4471, 9.5406, 1.5854, 2.8460, 8.9391, 1.7316, 1.1745, 9.1514, 0.9674, -1.1230, 10.8830, -0.8673
};

int main(int argc, char **argv) {
    
    signal_t signal;            // Wrapper for raw input buffer
    ei_impulse_result_t result; // Used to store inference output
    EI_IMPULSE_ERROR res;       // Return code from inference

    // Calculate the length of the buffer
    size_t buf_len = sizeof(input_buf) / sizeof(input_buf[0]);

    // Make sure that the length of the buffer matches expected input length
    if (buf_len != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        printf("ERROR: The size of the input buffer is not correct.\r\n");
        printf("Expected %d items, but got %d\r\n", 
                EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, 
                (int)buf_len);
        return 1;
    }

    // Assign callback function to fill buffer used for preprocessing/inference
    signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    signal.get_data = &get_signal_data;

    // Perform DSP pre-processing and inference
    res = run_classifier(&signal, &result, false);

    // Print return code and how long it took to perform inference
    printf("run_classifier returned: %d\r\n", res);
    printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n", 
            result.timing.dsp, 
            result.timing.classification, 
            result.timing.anomaly);

    // Print the prediction results (object detection)
#if EI_CLASSIFIER_OBJECT_DETECTION == 1
    printf("Object detection bounding boxes:\r\n");
    for (uint32_t i = 0; i < EI_CLASSIFIER_OBJECT_DETECTION_COUNT; i++) {
        ei_impulse_result_bounding_box_t bb = result.bounding_boxes[i];
        if (bb.value == 0) {
            continue;
        }
        printf("  %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\r\n", 
                bb.label, 
                bb.value, 
                bb.x, 
                bb.y, 
                bb.width, 
                bb.height);
    }

    // Print the prediction results (classification)
#else
    printf("Predictions:\r\n");
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        printf("  %s: ", ei_classifier_inferencing_categories[i]);
        printf("%.5f\r\n", result.classification[i].value);
    }
#endif

    // Print anomaly result (if it exists)
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    printf("Anomaly prediction: %.3f\r\n", result.anomaly);
#endif

    return 0;
}

// Callback: fill a section of the out_ptr buffer when requested
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (input_buf + offset)[i];
    }

    return EIDSP_OK;
}