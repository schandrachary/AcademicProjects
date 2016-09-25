#include "HandGesture.hpp"

using namespace std;
using namespace polysync;

//Default constructor.
HandGesture::HandGesture(int argc, char * argv[]) :
    Node(argc, argv)
{
}

//Default destructor.
HandGesture::~HandGesture()
{
}

//Can handle arguments here, if using the constructor with command line arguments.
void HandGesture::setConfigurationEvent(int argc, char * argv[])
{
    int c;
    bool exit = false;

    optind = 0; //Resets the parsing index for getopt.
    opterr = 1; //Re-enables showing an error for invalid parameters.

    while ((c = getopt(argc, argv, "hn:o")) != -1)
    {
        switch (c)
        {
            case 'h':
                cout << endl;
                cout << "Empty PolySync 2.x C++ Application" << endl;
                cout << "Usage: HandGesture [options]" << endl;
                cout << "     -h            Show this help menu and exit." << endl;
                cout << "     -n <node_id>  The SDF Node Id (only if defined in the SDF Configurator)." << endl;
                cout << "     -o            Enables writing logging messages to stdout in addition to the PolySync log file." << endl;
                cout << endl;
                exit = true;
            case 'n':
                break;
            case 'o':
                break;
            case '?':
                exit = true;
            default:
                break;
        }
    }

    if (exit)
        disconnectPolySync();
}

//Register listeners for message types here.
void HandGesture::initStateEvent()
{
    //How to register message listeners.
    ps_msg_type type = getMessageTypeByName("ps_byte_array_msg");
    registerListener(type);
}

//Add time-dependent message publishing here.
//Node enters this state on every loop if not in another state.
void HandGesture::okStateEvent()
{
    //Insert a pause so we don't use 100% CPU time.
    this_thread::sleep_for(chrono::milliseconds(20));
}

void HandGesture::warnStateEvent()
{
}

void HandGesture::errorStateEvent()
{
}

void HandGesture::fatalStateEvent()
{
}

//Clean up resources in this function.
void HandGesture::releaseStateEvent()
{
}

//Handle incoming messages here. This is handled in a
//separate node from the main thread and will not block any other state.
void HandGesture::messageEvent(shared_ptr<Message> message)
{
    //Example of message filtering.
    if (auto byteMsg = datamodel::getSubclass<datamodel::ByteArrayMessage>(message))
    {
        byteMsg->print();
    }
}
