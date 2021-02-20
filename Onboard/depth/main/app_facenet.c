/* ESPRESSIF MIT License
 * 
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 * 
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <string.h>
#include <math.h>
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "app_facenet.h"
#include "sdkconfig.h"
#include "image_util.h"

#define OBJECT_LENGTH 0.24
#define PIXEL_LENGTH 110
#define DISTANCE 0.6
#define I_0 120
#define J_0 170

static const char *TAG = "app_process";

 mtmn_config_t init_config()
 {
     mtmn_config_t mtmn_config = {0};
     mtmn_config.type = FAST;
     mtmn_config.min_face = 80;
     mtmn_config.pyramid = 0.707;
     mtmn_config.pyramid_times = 4;
     mtmn_config.p_threshold.score = 0.6;
     mtmn_config.p_threshold.nms = 0.7;
     mtmn_config.p_threshold.candidate_number = 20;
     mtmn_config.r_threshold.score = 0.7;
     mtmn_config.r_threshold.nms = 0.7;
     mtmn_config.r_threshold.candidate_number = 10;
     mtmn_config.o_threshold.score = 0.7;
     mtmn_config.o_threshold.nms = 0.7;
     mtmn_config.o_threshold.candidate_number = 1;

     return mtmn_config;
 }


float x_hat(float olength,float ip,float i0,float pflength)
{
	return (olength * (ip - i0)) / pflength;
}

float y_hat(float olength, float jp, float j0, float pflength)
{
	return  (olength * (jp - j0)) / pflength;
}

float z_hat(float d, float plength, float pflength)
{
	return d*(plength/pflength);
}

float measure(float d, float olength, float ip, float jp, float i0,float j0,float pflength,float plength)
{
	float x_cor = x_hat(olength, ip, i0, pflength);
	float z_cor = z_hat(d,plength, pflength);
	float distance = pow(x_cor * x_cor + z_cor * z_cor,0.5);
	return distance; 
}


void print_array(dl_matrix3du_t *image_matrix)
{
	
	printf("[IMAGE]");
	// Dimensions of the image
	int w = image_matrix -> w;
	int h = image_matrix -> h;
	int c = image_matrix -> c;
	int count = w * h;
	uint8_t* r = image_matrix -> item;
	uint8_t* g = r + 1;
	uint8_t* b = r + 2;

	for (int i = 0; i < count; i++)
	{
		int val = ( 19595 * (*r) + 38469 * (*g) + 7472 * (*b)) >> 16;
		printf("%d ",val);
		if ((i+1) % w == 0)
		{
			printf(" \n");
		}
		r += 3;
		g += 3;
		b += 3;
	}
	return;
}

void task_process (void *arg)
{/*{{{*/
    size_t frame_num = 0;
    dl_matrix3du_t *image_matrix = NULL;
    camera_fb_t *fb = NULL;
    
    /* 1. Load configuration for detection */
     mtmn_config_t mtmn_config = init_config();

    do
    {
        int64_t start_time = esp_timer_get_time();
        /* 2. Get one image with camera */
        fb = esp_camera_fb_get();
        if (!fb)
        {
            ESP_LOGE(TAG, "Camera capture failed");
            continue;
        }
        int64_t fb_get_time = esp_timer_get_time();
        //ESP_LOGI(TAG, "Get one frame in %lld ms.", (fb_get_time - start_time) / 1000);

	//printf("Length of the frame %d\n",fb->len);

        /* 3. Allocate image matrix to store RGB data */
        image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
	//printf("Format %d \n",fb->format);

        /* 4. Transform image to RGB */
        uint32_t res = fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item);
        if (true != res)
        {
            ESP_LOGE(TAG, "fmt2rgb888 failed, fb: %d", fb->len);
            dl_matrix3du_free(image_matrix);
            continue;
        }
	
	//printf("Dimensions of the image - Width: - %d , Height - %d, Channel - %d, N - %d, Stride - %d, Data - %d \n", image_matrix->w,image_matrix->h,image_matrix->c,image_matrix->n,image_matrix->stride,*(image_matrix->item)); 

        esp_camera_fb_return(fb);

        /* 5. Do face detection */
         box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config);
        //ESP_LOGI(TAG, "Detection time consumption: %lldms", (esp_timer_get_time() - fb_get_time) / 1000);

         if (net_boxes)
         {
             frame_num++;
             //ESP_LOGI(TAG, "DETECTED: %d\n", frame_num);
	     printf("BOXES %f %f %f %f \n",net_boxes->box->box_p[0],net_boxes->box->box_p[1],net_boxes->box->box_p[2],net_boxes->box->box_p[3]);
	     float x_1 = net_boxes->box->box_p[0];
             float y_1 = net_boxes->box->box_p[1];
             float x_2 = net_boxes->box->box_p[2];
             float y_2 = net_boxes->box->box_p[3];
	     float pflength = y_2 - y_1;
             float ip = (x_2 - x_1) / 2;
             float jp = (y_2 - y_1) / 2;
             float distance = measure(DISTANCE, OBJECT_LENGTH, ip, jp, I_0, J_0, pflength, PIXEL_LENGTH);
	     printf("relative distance %f \n",distance);
	     dl_lib_free(net_boxes->score);
             dl_lib_free(net_boxes->box);
             dl_lib_free(net_boxes->landmark);
             dl_lib_free(net_boxes);
         }

	//print_array(image_matrix);
        dl_matrix3du_free(image_matrix);

    } while(1);
}/*}}}*/


void app_facenet_main()
{
    xTaskCreatePinnedToCore(task_process, "process", 4 * 1024, NULL, 5, NULL, 1);
}
