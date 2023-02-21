#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include <Field.h>
#include <Shader.h>

class Environment
{
    public:
        Environment();
        virtual ~Environment();
        int screen_width;
        int screen_height;
        double last_time;
        double t;
        double build_up_time;
        vector < Field* > fields;
        Shader* shader;
        unsigned int num_of_points;
    protected:

    private:
};

#endif // ENVIRONMENT_H

