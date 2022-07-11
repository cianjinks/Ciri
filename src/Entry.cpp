#include "Application.h"

int main(int argc, char *argv[])
{
    Ciri::Application *app = new Ciri::Application();
    app->Run();
    delete app;
}