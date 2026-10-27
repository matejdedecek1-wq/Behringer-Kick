
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include "ImGuiFileDialog.h"
#include <stdio.h>
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>
#include "DeviceManager.h"
#include "../Includes/Design.h"
#include "SFSynth.h"
#include "StereoOut.h"

#define TML_IMPLEMENTATION
#include "tml.h"
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <thread>
#include <chrono>

AudIO::DeviceManager devmgr;
AudIO::Design* pDesign = nullptr;
// Holds global MIDI playback state
static double       g_Msec = 0; //current playback time
tml_message* g_MidiMessage = nullptr; //next message to be played
AudIO::SFSynth* pSynth1 = nullptr;
AudIO::StereoOut* pStereoOut = nullptr;
#define MEM_POOL_SIZE 20
#define SAMPLE_RATE   48000
#define BLOCK_SIZE    960

long                                           noteOnCount = 0;
long                                           noteOffCount = 0;
std::chrono::high_resolution_clock::time_point start;

// NOTE::
// This buffer will be used by AudIO to create audio graph.
// Ensure that this has sufficient storage to accomodate worst case design requirement.
float _audioPoolMemBuffer[(MEM_POOL_SIZE * BLOCK_SIZE)];

void handleMsg( tml_message* msg, AudIO::SFSynth* synth )
{
    switch( msg->type )
    {
        case TML_PROGRAM_CHANGE:
        synth->programChange( msg->program, msg->channel + 1 );
        break;
        case TML_NOTE_ON: //play a note
        synth->channelNoteOn( msg->key, msg->velocity, msg->channel + 1 );
        noteOnCount++;
        break;
        case TML_NOTE_OFF: //stop a note
        synth->channelNoteOff( msg->key, msg->channel + 1 );
        noteOffCount++;
        break;
        case TML_PITCH_BEND: //pitch wheel modification
        synth->pitchBend( msg->pitch_bend, msg->channel + 1 );
        break;
        case TML_CONTROL_CHANGE: //MIDI controller messages
        synth->controlChange( msg->control, msg->control_value, msg->channel + 1 );
        break;
        case TML_ALL_NOTES_OFF:
        synth->noteOffAll();
        break;
        default:
        break;
    }
}

const char* Application_GetName( void )
{
    return "SynthSimulator";
}

static int   filterFc = 0, resonance = 0, attack = 0, release = 0, reverb = 0, chorus = 0, eqlow = 0, eqhigh = 0, volume = 100;
std::string filename = "";
static int pitch = 0x40;
static int velocity = 0x60;
static int channel = 1;

void         Application_Frame( void )
{
    ImGui::BeginGroup();
    {
        if( ImGui::Button( "Load Midi File" ) )
        {
            ImGuiFileDialog::Instance()->OpenDialog( "Select Midi File", "Choose File", ".mid", "." );
        }
        // display
        if( ImGuiFileDialog::Instance()->Display( "Select Midi File" ) )
        {
            // action if OK
            if( ImGuiFileDialog::Instance()->IsOk() )
            {
                filename = ImGuiFileDialog::Instance()->GetFilePathName();
            }
            // close
            ImGuiFileDialog::Instance()->Close();
        }
        ImGui::SameLine();
        ImGui::Text( filename.c_str() );
        ImGui::Dummy( ImVec2( 0, 10 ) );
        ImGui::PushItemWidth( 200 );
        if( ImGui::SliderInt( "Filter Cutoff", &filterFc, 0, 0x7f ) )
        {
            pSynth1->setLowPassCutoff( filterFc, 1 );
        }
        ImGui::Dummy( ImVec2( 0, 10 ) );
        if( ImGui::SliderInt( "Filter Resonance", &resonance, 0, 0x7f ) )
        {
            pSynth1->setLowPassResonance( resonance, 1 );
        }
        ImGui::Dummy( ImVec2( 0, 10 ) );
        if( ImGui::SliderInt( "Filter Attack", &attack, 0, 0x7f ) )
        {
            pSynth1->setLowPassAttack( attack, 1 );
        }
        ImGui::Dummy( ImVec2( 0, 10 ) );
        if( ImGui::SliderInt( "Filter Release", &release, 0, 0x7f ) )
        {
            pSynth1->setLowPassRelease( release, 1 );
        }
        ImGui::Dummy( ImVec2( 0, 10 ) );
        if( ImGui::SliderInt( "Reverb", &reverb, 0, 0x7f ) )
        {
            pSynth1->setReverb( reverb, 1 );
        }
        ImGui::Dummy( ImVec2( 0, 10 ) );
        if( ImGui::SliderInt( "Chorus", &chorus, 0, 0x7f ) )
        {
            pSynth1->setChorus( chorus, 1 );
        }
        ImGui::Dummy( ImVec2( 0, 10 ) );
        if( ImGui::SliderInt( "Eq Low", &eqlow, 0, 0x7f ) )
        {
            pSynth1->setEqLow( eqlow, 1 );
        }
        ImGui::Dummy( ImVec2( 0, 10 ) );
        if( ImGui::SliderInt( "Eq High", &eqhigh, 0, 0x7f ) )
        {
            pSynth1->setEqHigh( eqhigh, 1 );
        }
        ImGui::Dummy( ImVec2( 0, 10 ) );
        if( ImGui::SliderInt( "Volume", &volume, 0, 100 ) )
        {
            pSynth1->setGlobalGainDb( volume - 100 );
        }
        ImGui::Dummy( ImVec2( 0, 10 ) );

        ImGui::SliderInt( "Pitch", &pitch, 0, 0x7f );

        ImGui::SameLine();

        ImGui::SliderInt( "Velocity", &pitch, 0, 0x7f );

        ImGui::SameLine();

        ImGui::SliderInt( "Channel", &channel, 1, 16 );

        ImGui::SameLine();

        if( ImGui::Button( "Note On" ) )
        {
            pSynth1->channelNoteOn( pitch, velocity, channel );
        }

        ImGui::SameLine();

        if( ImGui::Button( "Note Off" ) )
        {
            pSynth1->channelNoteOff( pitch, channel );
        }

        ImGui::SameLine();

        if( ImGui::Button( "Note Off All" ) )
        {
            pSynth1->noteOffAll();
        }

        ImGui::Dummy( ImVec2( 0, 10 ) );

        ImGui::PopItemWidth();
        ImGui::EndGroup();
    }
    if( filename != "" )
    {
        if( g_MidiMessage )
        {
            g_Msec = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
            for( ; g_MidiMessage && g_Msec >= g_MidiMessage->time; g_MidiMessage = g_MidiMessage->next )
            {
                handleMsg( g_MidiMessage, pSynth1 );
            }
        } else
        {
            start = std::chrono::high_resolution_clock::now();
            g_MidiMessage = tml_load_filename( filename.c_str() );
        }
    }
}

void Application_Initialize( void )
{
    AudIO::MemoryPool<float>::get().initialize( _audioPoolMemBuffer, MEM_POOL_SIZE, BLOCK_SIZE );
    pStereoOut = new AudIO::StereoOut();
    devmgr.addAudioDevice( "StereoOut", pStereoOut, true );
    pDesign = new AudIO::Design( 0, 2 );
    auto synth1 = pDesign->createObject( "SFSynth" );
    pDesign->setDesignOutput( synth1 );
    auto stereoOut = devmgr.getAudioDevice( "StereoOut" );
    pDesign->setBypass( false );
    devmgr.setAudioCallback( pDesign );
    std::cout << "USED MEM: " << AudIO::MemoryPool<float>::get().getUsed() << std::endl;

    devmgr.start( SAMPLE_RATE, BLOCK_SIZE );
    pSynth1 = dynamic_cast<AudIO::SFSynth*>(pDesign->getObject( synth1 ));
}

void Application_Finalize( void )
{
    free( pDesign );
}

static void glfw_error_callback( int error, const char* description )
{
    fprintf( stderr, "Glfw Error %d: %s\n", error, description );
}



GLFWwindow* ImGuiSetup( void );
void IMGuiCleanup( GLFWwindow* window );

int main( int argc, char** /*argv[]*/ )
{
    auto window = ImGuiSetup();
    if( !window )
        return -1;
    Application_Initialize();
    ImVec4 clear_color = ImVec4( 0.45f, 0.55f, 0.60f, 1.00f );
    ImGuiIO& io = ImGui::GetIO();
    // Main loop
    while( !glfwWindowShouldClose( window ) )
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );
        ImGui::SetNextWindowSize( io.DisplaySize );
        ImGui::Begin( "Content", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoBringToFrontOnFocus );

        Application_Frame();
        ImGui::End();
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize( window, &display_w, &display_h );
        glViewport( 0, 0, display_w, display_h );
        glClearColor( clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w );
        glClear( GL_COLOR_BUFFER_BIT );

        // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
        // you may need to backup/reset/restore other state, e.g. for current shader using the commented lines below.
        //GLint last_program;
        //glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        //glUseProgram(0);
        ImGui_ImplOpenGL2_RenderDrawData( ImGui::GetDrawData() );
        //glUseProgram(last_program);

        glfwMakeContextCurrent( window );
        glfwSwapBuffers( window );
    }
    Application_Finalize();
    IMGuiCleanup( window );
    return 0;

}

GLFWwindow* ImGuiSetup( void )
{
    // Setup window
    glfwSetErrorCallback( glfw_error_callback );
    if( !glfwInit() )
        return 0;
    GLFWwindow* window = glfwCreateWindow( 1280, 720, Application_GetName(), NULL, NULL );
    if( window == NULL )
        return 0;
    glfwMakeContextCurrent( window );
    glfwSwapInterval( 1 ); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL( window, true );
    ImGui_ImplOpenGL2_Init();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
    return window;
}

void IMGuiCleanup( GLFWwindow* window )
{
    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow( window );
    glfwTerminate();
}

