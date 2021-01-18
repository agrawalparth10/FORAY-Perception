/*
    Modified Version of Examples from the Eloquent Library
    https://github.com/eloquentarduino/EloquentTinyML
*/


#include <EloquentTinyML.h>
#include <model.h>


// Framesize taken from the internet  - FRAMESIZE_QQVGA,    // 160x120
// Can be Changed 
#define INPUTS 64
#define OUTPUTS 2
#define TENSOR_ARENA_SIZE 1*1024

Eloquent::TinyML::TfLite<INPUTS,OUTPUTS,TENSOR_ARENA_SIZE> ml;

void setup() {
    Serial.begin(115200);
    ml.begin(model) // Model Name from the model.h file
}

void loop() {

    float x_test[INPUTS] = {0};

    // Allocating Space for prediction 
    float y_pred[OUTPUTS] = {0};

    //Prediction 
    ml.predict(x_test,y_pred);


    for (int i = 0; i < INPUTS; i++)
    {
        Serial.print(y_pred[i]);
        Serial.print(",");
    }
    Serial.println();

    Serial.print("Predicted");
    Serial.println(ml.probaToClass(y_pred));
    Serial.print("Actual");
    Serial.print(ml.predictClass(x_test))    
}