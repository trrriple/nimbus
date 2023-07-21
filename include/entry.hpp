#pragma once

extern nimbus::Application* nimbus::createApplication();

int nimbusEntry()
{
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
    (void)(argc);
    (void)(argv);
    return nimbusEntry();
}
#else
int main(int argc, char* argv[])
{
    (void)(argc);
    (void)(argv);

    return nimbusEntry();

    return 0;
}

#endif