/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <Arduino.h>
#include <M5Unified.h>
#include <M5ModuleLLM.h>

M5ModuleLLM module_llm;
String tts_work_id;

void setup()
{
    M5.begin();
    M5.Display.setTextSize(2);
    M5.Display.setTextScroll(true);

    /* Init module serial port */
    Serial2.begin(115200, SERIAL_8N1, 16, 17);  // Basic
    // Serial2.begin(115200, SERIAL_8N1, 13, 14);  // Core2
    // Serial2.begin(115200, SERIAL_8N1, 18, 17);  // CoreS3

    /* Init module */
    module_llm.begin(&Serial2);

    /* Make sure module is connected */
    M5.Display.printf(">> Check ModuleLLM connection..\n");
    while (1) {
        if (module_llm.checkConnection()) {
            break;
        }
    }

    /* Reset ModuleLLM */
    M5.Display.printf(">> Reset ModuleLLM..\n");
    module_llm.sys.reset();

    /* Setup Audio module */
    M5.Display.printf(">> Setup audio..\n");
    module_llm.audio.setup();

    /* Setup TTS module and save returned work id */
    M5.Display.printf(">> Setup tts..\n\n");
    tts_work_id = module_llm.tts.setup();
}

void loop()
{
    /* Make a text for speech: {i} plus {i} equals to {i + i} */
    static int i = 0;
    i++;
    std::string text = std::to_string(i) + " plus " + std::to_string(i) + " equals " + std::to_string(i + i) + ".";

    M5.Display.setTextColor(TFT_GREEN);
    M5.Display.printf("<< %s\n\n", text.c_str());

    /* Push text to TTS module and wait inference result */
    module_llm.tts.inference(tts_work_id, text.c_str(), 10000);

    delay(500);
}