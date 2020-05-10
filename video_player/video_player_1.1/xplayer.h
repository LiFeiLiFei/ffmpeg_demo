#ifndef _X_PLAYER_H_
#define _X_PLAYER_H_

#include <SDL.h>

#include "x_ffmepg_demultiplexer.h"
#include "x_ffmpeg_video_decoder.h"
#include "x_ffmpeg_audio_decoder.h"
#include "x_ffmpeg_av_sync.h"

#include "xframe.h"

class xPlayer :
    public x::ffmpeg::xDemultiplexEvent,
    public x::ffmpeg::xDecoderEvent
{
private:
    // ȫ���Ƿ����б�� ���������߳̽���
    volatile bool run_flag_;
private:
    //SDL2���

    SDL_AudioSpec s_aspec_;
    SDL_Window* s_win_;
    SDL_Renderer* s_render;
    SDL_Texture* s_txture;
    SDL_Rect s_rect_;

    // ������Ƶ�߳�
    std::thread play_video_thread_;
private:
    x::ffmpeg::xAVSync av_sync_;    //����Ƶͬ��ģ��
    x::ffmpeg::xDemultiplexer dux_; //�⸴����ģ��
    x::ffmpeg::xVideoDecoder vdec_; //��Ƶ������ģ��
    x::ffmpeg::xAudioDecoder adec_; //��Ƶ������ģ��
    x::xQueue<xPtrFrame> qvdata_;   //��Ƶ�������������
    x::xQueue<xPtrFrame> qadata_;   //��Ƶ�������������
public:
    xPlayer();
    ~xPlayer();
private:
    //�⸴�ÿ�ʼ�ص�
    void DuxStart();

    //�⸴�÷���pakcet�Ļص�
    void DuxPacket(AVPacket* _data, int _type);

    //�⸴�ý���
    void DuxEnd();
private:
    //����������frame�Ļص�
    void VideoEvent(AVFrame* _img, x::ffmpeg::xVideoDecoder* _decoder);
    void AudioEvent(uint8_t* _buf, int _len, int64_t _pts, x::ffmpeg::xAudioDecoder* _decoder);
private:
    //����ͼ���߳�
    void play_video_function();

    //SDL��Ϣ����
    void sdl2_event_function();
private:
    //��ffempg���
    bool open_ffmepg(const char* _inpu);

    //��sdl2���
    bool open_sdl2();
public:
    //��¶�ڲ���Ա���� ��Ƶ������� ��sdl��audio�ص�ʹ��
    x::xQueue<xPtrFrame>& AudioDataQueue();

    // ��¶�ڲ���Ա���� ����Ƶͬ�� ��sdl��audio�ص�ʹ��
    x::ffmpeg::xAVSync& AVSync();
public:
    //����player
    bool Start(const char* _input);

    //ֹͣplayer��������ر���
    void Stop();
};

#endif //_X_PLAYER_H_