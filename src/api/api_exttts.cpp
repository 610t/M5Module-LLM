/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "api_exttts.h"

using namespace m5_module_llm;

void ApiExttts::init(ModuleMsg* moduleMsg)
{
    _module_msg = moduleMsg;
}

String ApiExttts::setup(ApiExtttsSetupConfig_t config, String request_id, String language)
{
    String cmd;
    {
        JsonDocument doc;
        doc["request_id"]              = request_id;
        doc["work_id"]                 = "exttts";
        doc["action"]                  = "setup";
        doc["object"]                  = "exttts.setup";
        doc["data"]["model"]           = config.model;
        doc["data"]["cmdtype"]         = "open_jtalk";
        doc["data"]["response_format"] = config.response_format;
        JsonArray inputArray           = doc["data"]["input"].to<JsonArray>();
        for (const String& str : config.input) {
            inputArray.add(str);
        }
        if (language == "zh_CN") doc["data"]["model"] = "";
        doc["data"]["enoutput"] = config.enoutput;
        doc["data"]["enaudio"]  = config.enaudio;
        serializeJson(doc, cmd);
    }

    String work_id;
    _module_msg->sendCmdAndWaitToTakeMsg(
        cmd.c_str(), request_id,
        [&work_id](ResponseMsg_t& msg) {
            // Copy work id
            work_id = msg.work_id;
        },
        15000);
    return work_id;
}

int ApiExttts::inference(String work_id, String input, uint32_t timeout, String request_id)
{
    String cmd;
    {
        JsonDocument doc;
        doc["request_id"]     = request_id;
        doc["work_id"]        = work_id;
        doc["action"]         = "inference";
        doc["object"]         = "tts.utf-8.stream";
        doc["data"]["delta"]  = input;
        doc["data"]["index"]  = 0;
        doc["data"]["finish"] = true;
        serializeJson(doc, cmd);
    }

    if (timeout == 0) {
        _module_msg->sendCmd(cmd.c_str());
        return MODULE_LLM_OK;
    }

    int ret = MODULE_LLM_WAIT_RESPONSE_TIMEOUT;
    _module_msg->sendCmdAndWaitToTakeMsg(
        cmd.c_str(), request_id,
        [&ret](ResponseMsg_t& msg) {
            // Copy error code
            ret = msg.error.code;
        },
        timeout);
    return ret;
}
