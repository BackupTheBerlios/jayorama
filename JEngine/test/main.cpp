// #include <maddecoder.h>
// #include <unistd.h>
#include <iostream>
// #include <oggfiledecoder.h>
#include <ossoutput.h>
#include <sndfiledecoder.h>
#include <mp3filedecoder.h>
#include <aoggfiledecoder.h>


using namespace izsound;
using namespace std;

int main()
{
//     char *p = "/home/j_zar/music/07-Traccia Audio 07.mp3";
    char *p = "/home/j_zar/music/grignani_-_laiuolarmx_p2p_ext_rmx.ogg";
//     OggFileDecoder decoder;
//     ApOutput oss;
//     SndFileDecoder decoder;
//     Mp3FileDecoder decoder( 65036, 44100 );
    AOggFileDecoder decoder;
    OssOutput oss( 44100, 500, "/dev/dsp1" );
    cout << "Ok apout" << endl;
//     ApFileDecoder ap;
//     MadDecoder decoder;
//     ap.open( p );
//     cout << "Ok ap" << endl;
    decoder.connect(&oss, 0, 0);
    decoder.play();
    decoder.open(p);
//     decoder.pause();
    int frames = decoder.getFrames();
    cout << "Frames = " << frames << endl;
    decoder.setFrame( frames - 1000000 );
    decoder.setReadDirection( BACKWARD );
    decoder.play();
    cout << "Length = " << decoder.getTotalTime() << endl;
    for (int i = 0; i < 6000; ++i) decoder.run();
    decoder.disconnect(0);
    cout << "Ok end" << endl;    
//     sleep(3);
    return 1;
} 
