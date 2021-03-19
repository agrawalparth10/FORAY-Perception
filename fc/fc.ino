#define CAMERA_MODEL_ESP_EYE
//#define FRAME_SIZE FRAMESIZE_QQVGA2
//#define WIDTH 128
//#define HEIGHT 160
#define FRAME_SIZE FRAMESIZE_QQVGA // 160 X 120
#define WIDTH 160
#define HEIGHT 120
#define CHANNEL 3
//#define INPUTS 64 // change
#define INPUTS 40*30*3
#define OUTPUTS 2 //change
#define TENSOR_ARENA_SIZE 80*1024 // change
#define DEBUG 0

#include "camera_pins.h"
#include "esp_camera.h"
#include "model.h"
#include "EloquentTinyML.h"
#include "C:\Users\HP\Documents\Arduino\libraries\lib\include\dl_lib_matrix3d.h"// matrix library



bool setup_camera(framesize_t frameSize) 
{
    camera_config_t config;

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;  
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG; // Change the format of image
    config.frame_size = frameSize; // Framesize
    config.jpeg_quality = 10;
    config.fb_count = 1;

    bool ok = esp_camera_init(&config) == ESP_OK;

    sensor_t *sensor = esp_camera_sensor_get();
    sensor->set_framesize(sensor, frameSize);

    return ok;
}


Eloquent::TinyML::TfLite<INPUTS,OUTPUTS,TENSOR_ARENA_SIZE> ml;

dl_matrix3du_t * image_matrix = NULL;
dl_matrix3du_t * image_resize_matrix = NULL;


void print_array(dl_matrix3du_t *image_matrix)
{
    printf("[IMAGE]");

    // Dimensions of the image
    int w = image_matrix -> w; // Width
    int h = image_matrix -> h; // Height
    int c = image_matrix -> c; // Channel
    int count = w * h;
    unsigned char* r = image_matrix -> item;
    unsigned char* g = r + 1;
    unsigned char* b = r + 2;

    for (int i = 0; i < count; i++)
    {
    int val = ( 19595 * (*r) + 38469 * (*g) + 7472 * (*b)) >> 16; // Convert to GrayScale
    printf("%d ",val); 
    if ((i+1) % w == 0)
    {
        printf("\n");
    }
    r += 3;
    g += 3;
    b += 3;
    }
    return;
}


bool capture_still()
{
    camera_fb_t *frame = esp_camera_fb_get();

    if(!frame)
    {
        return false;
    }

    // Convert the frame buffer [JPEG -> Array] and store it in image_matrix


    fmt2rgb888(frame->buf, frame->len, frame->format, image_matrix->item);

    Serial.print("THIS RAN");
    image_resize_matrix->w = 40;
    image_resize_matrix->h = 30;
    image_resize_matrix->c = 3;
    image_resize_matrix->stride = image_matrix->stride;    

    image_resize_linear(image_resize_matrix->item,image_matrix->item,40,30,3,160,120);
    
    // Print the Array
    if (DEBUG)
    {
      print_array(image_matrix);
      Serial.println("NEW");
      print_array(image_resize_matrix);
    }
    return true;
}

void setup()
{
    Serial.begin(115200);
    Serial.println(setup_camera(FRAME_SIZE) ? "OK INIT" : "ERROR");
    ml.begin(model_data); // Name of the array in the model.h file
    image_matrix = dl_matrix3du_alloc(1,WIDTH,HEIGHT,CHANNEL);
    image_resize_matrix = dl_matrix3du_alloc(1,40,30,CHANNEL);
    Serial.println("TEST START\n");
}


void loop()
{
    printf("TESTING \n");
    
    if(!capture_still())
    {
        printf("[ERROR] Capture Failed!");
        delay(2000);
        return;
    }


    float y_pred[OUTPUTS] = {0}; 
    /*float x_test[64] = { 0.    , 0.    , 0.625 , 0.875 , 0.5   , 0.0625, 0.    , 0.    ,
                         0.    , 0.125 , 1.    , 0.875 , 0.375 , 0.0625, 0.    , 0.    ,
                         0.    , 0.    , 0.9375, 0.9375, 0.5   , 0.9375, 0.    , 0.    ,
                         0.    , 0.    , 0.3125, 1.    , 1.    , 0.625 , 0.    , 0.    ,
                         0.    , 0.    , 0.75  , 0.9375, 0.9375, 0.75  , 0.    , 0.    ,
                         0.    , 0.25  , 1.    , 0.375 , 0.25  , 1.    , 0.375 , 0.    ,
                         0.    , 0.5   , 1.    , 0.625 , 0.5   , 1.    , 0.5   , 0.    ,
                         0.    , 0.0625, 0.5   , 0.75  , 0.875 , 0.75  , 0.0625, 0.    };
*/
    uint8_t * test = image_resize_matrix->item;
    ml.predict(test,y_pred);

    for(int i = 0; i < OUTPUTS; i++)
    {
        Serial.print(y_pred[i]);
        Serial.print(" ");
    }
    printf("\n");

    delay(2000);
}
