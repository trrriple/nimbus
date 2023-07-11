#pragma once

extern nimbus::Application* nimbus::createApplication();

int main(int argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    
    auto app = nimbus::createApplication();

    app->onInit();
    app->execute();
    app->onExit();
    delete app;

    return 0;
}

#ifdef _WIN32
int WinMain(int argc, char* argv[])
{   
    main(argc, argv);

    return 0;
}
#endif