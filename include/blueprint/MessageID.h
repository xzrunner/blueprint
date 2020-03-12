#pragma once

namespace bp
{

enum MessageID
{
    MSG_BP_CONN_INSERT         = 4000,
    MSG_BP_CONN_DELETE         = 4001,
    MSG_BP_CONN_REBUILD        = 4002,

    MSG_BP_NODE_PROP_CHANGED   = 4003,

    MSG_BP_DISABLE_UPDATE_EVAL = 4004,
    MSG_BP_ENABLE_UPDATE_EVAL  = 4005,

}; // MessageID

}