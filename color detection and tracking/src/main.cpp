#include "HandGesture.hpp"
#include <iostream>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int c;

    bool gaveSdfId = false;
    int sdfId;
    bool stdLogging = false;

    opterr = 0; //Sets the following getopt to ignore invalid parameters.

    while((c = getopt(argc, argv, "+n:o")) != -1)
    {
        switch(c)
        {
            case 'n':
            {
                sdfId = strtoul(optarg, NULL, 10);
                if (sdfId > 0)
                    gaveSdfId = true;
            } break;
            case 'o':
            {
                stdLogging = true;
            } break;
            case '?':
                break;
            default:
                break;
        }
    }

    HandGesture mn(argc, argv);
    mn.setNodeType(PSYNC_NODE_TYPE_API_USER);
    mn.setNodeName("HandGesture");

    if (gaveSdfId)
        mn.setSDFID(sdfId);

    if (stdLogging)
        mn.setFlags(PSYNC_INIT_FLAG_STDOUT_LOGGING);

    mn.connectPolySync();

    return 0;
}
