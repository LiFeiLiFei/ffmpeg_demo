#include <stdio.h>

#include "xplayer.h"

int main(int _argc, char* _argv[])
{
    xPlayer player;
    player.Start("../../test_video/ʧ�߷���.mp4");
    player.Stop();
    return 0;
}