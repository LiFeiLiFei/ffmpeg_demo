#include <stdio.h>
#include <Windows.h>
#include "xplayer.h"

int main(int _argc, char* _argv[])
{
    SetProcessDPIAware(); //���windows ��������

    xPlayer player;

    player.Start("../../test_video/ʧ�߷���.mp4");
    player.Stop();
    return 0;
}