/**
    main.cpp

    The main entry point.

    Copyright (c) 2020 D. Scott Miller. All rights reserved.
    This code is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation; either version 2 of the License, or (at your option)
    any later version.
*/

#include "h5000-logger-class.h"

/**
 * @brief Main entry point for the program.
 * 
 * @param argc Number of arguments supplied on the command line.
 * @param argv Array of strings representing command line arguments.
 * @return ERROR_SUCCESS if successful; otherwise, an error code.
*/
int main(int argc, char** argv)
{
    // Instantiate the application (with command line parameters)
    H5000Logger app(argc, argv);

    // Execute the application
    return app.run();
}
