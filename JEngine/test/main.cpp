
#include <unistd.h>
#include <iostream>
#include <ossoutput.h>
#include <formats/sndfiledecoder.h>
#include <oggfiledecoder.h>


using namespace izsound;
using namespace std;

int main()
{
//     char *p = "/home/j_zar/music/07-Traccia Audio 07.mp3";
//     char *p = "/home/j_zar/music/grignani_-_laiuolarmx_p2p_ext_rmx.ogg";
    char *p = "/home/j_zar/music/traccia_13.wav";
//     ApOutput oss;
    SndFileDecoder decoder;
//     OggFileDecoder decoder;
    OssOutput oss( 44100, 500, "/dev/dsp1" );
    cout << "Ok apout" << endl;
//     ApFileDecoder ap;
//     MadDecoder decoder;
//     ap.open( p );
//     cout << "Ok ap" << endl;
    decoder.connect(&oss, 0, 0);
//     decoder.play();
    decoder.open(p);
//     decoder.pause();
//     if ( decoder.status() == DECODER_OK )
//     	cout << "Decoder ok" << endl;
//     else
//     	cout << "Decoder broken" << endl;
    unsigned int frames = decoder.getFrames();
    cout << "Frames = " << frames << endl;
    decoder.play();
//     sleep(3);
    decoder.setFrame( frames - 5000000 );
    decoder.setReadDirection( BACKWARD );
//     cout << "Length = " << decoder.getTotalTime() << endl;
    for (int i = 0; i < 6000; ++i) /*decoder.run()*/sleep(1);
    // stop first avoids seg-fault!
//     decoder.stop();
    decoder.disconnect(0);
    cout << "Ok end" << endl;    
//     sleep(3);
    return 1;
} 
