
#include <unistd.h>
#include <iostream>
// #include <ossoutput.h>
#include <sndfiledecoder.h>
#include <rtaudioapi.h>
#include <outputobjectbase.h>


using namespace izsound;
using namespace std;

int main()
{
//     char *p = "/home/j_zar/music/07-Traccia Audio 07.mp3";
//     char *p = "/home/j_zar/music/grignani_-_laiuolarmx_p2p_ext_rmx.ogg";
    char *p = "/home/j_zar/music/Nek_-_ci_6_tu.wav";
//     ApOutput oss;
    SndFileDecoder decoder;
//     MpegDecoder decoder;
    RtAudioAPI api;
    OutputObjectBase *out_object = api.getOutputUnit( 2, 2200, 4, 44100,-1 );
    sleep(1);
    OSEDeviceInfo info;
    api.getDeviceInfo( 1, &info );
    cout << "name = " << info.name << endl;
    cout << "Devices = " << api.getDeviceCount() << endl;
    cout << "outputChannels = " << info.outputChannels << endl;
    cout << "inputChannels = " << info.inputChannels << endl;
    
    
    cout << "Ok apout" << endl;
//     ApFileDecoder ap;
//     MadDecoder decoder;
//     ap.open( p );
//     cout << "Ok ap" << endl;
    decoder.connect(out_object, 0, 0);
//     decoder.play();
    cout << "Connected..." << endl;
    decoder.open(p);
    cout << "File opened..." << endl;
//     decoder.pause();
//     if ( decoder.status() == DECODER_OK )
//     	cout << "Decoder ok" << endl;
//     else
//     	cout << "Decoder broken" << endl;
    unsigned int frames = decoder.getFrames();
    cout << "Frames = " << frames << endl;
    cout << "Total time = " << decoder.getTotalTime() << endl;
    decoder.play();
//     sleep(3);
//     decoder.setCurrentTime( 30 );
//     decoder.setFrame( frames - 5000000 );
//     decoder.setReadDirection( BACKWARD );
//     cout << "Length = " << decoder.getTotalTime() << endl;
    for (int i = 0; i < 6000; ++i) /*decoder.run()*/sleep(1);
    // stop first avoids seg-fault!
//     decoder.stop();
    decoder.disconnect(0);
    cout << "Ok end" << endl;    
//     sleep(3);
    return 1;
} 
