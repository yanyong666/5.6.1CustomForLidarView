/*****************************************************************************
*
* Copyright (c) 2000 - 2018, Lawrence Livermore National Security, LLC
* Produced at the Lawrence Livermore National Laboratory
* LLNL-CODE-442911
* All rights reserved.
*
* This file is  part of VisIt. For  details, see https://visit.llnl.gov/.  The
* full copyright notice is contained in the file COPYRIGHT located at the root
* of the VisIt distribution or at http://www.llnl.gov/visit/copyright.html.
*
* Redistribution  and  use  in  source  and  binary  forms,  with  or  without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of  source code must  retain the above  copyright notice,
*    this list of conditions and the disclaimer below.
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
*    documentation and/or other materials provided with the distribution.
*  - Neither the name of  the LLNS/LLNL nor the names of  its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
* ARE  DISCLAIMED. IN  NO EVENT  SHALL LAWRENCE  LIVERMORE NATIONAL  SECURITY,
* LLC, THE  U.S.  DEPARTMENT OF  ENERGY  OR  CONTRIBUTORS BE  LIABLE  FOR  ANY
* DIRECT,  INDIRECT,   INCIDENTAL,   SPECIAL,   EXEMPLARY,  OR   CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR
* SERVICES; LOSS OF  USE, DATA, OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER
* CAUSED  AND  ON  ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT
* LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY  WAY
* OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
*****************************************************************************/

// ************************************************************************* //
//                              Environment.C                                //
// ************************************************************************* //
#ifndef _POSIX_C_SOURCE
#   define _POSIX_C_SOURCE 200112L
#endif
#include <cstdlib>
#include <cstring>
#include <errno.h>
#ifdef  _WIN32
# include <windows.h>
#endif

#include <Environment.h>

#include <DebugStream.h>
#include <snprintf.h>

namespace Environment {

// ****************************************************************************
//  Function: Environment::get
//
//  Purpose:  Obtains a value from the environment.
//            `getenv' appears to be pretty standard; we don't do anything
//            special here.
//
//  Programmer: Tom Fogal
//
// ****************************************************************************
std::string
get(const char *variable)
{
    const char *value = getenv(variable);
    if (value == NULL)
    {
        std::string str;
        return str;
    }
    return std::string(value);
}

// ****************************************************************************
//  Function: Environment::get
//
//  Purpose: Predicate to determine whether a variable is defined.
//
//  Programmer: Tom Fogal
//
// ****************************************************************************
bool
exists(const char *variable)
{
    char *value = getenv(variable);
    if(value == NULL)
    {
        return false;
    }
    return true;
}

// ****************************************************************************
//  Function: Environment::set
//
//  Purpose: Sets a value in the enviroment.  Avoid using putenv, since it
//           requires static memory.
//
//  Programmer: Tom Fogal
//
//  Modifications:
//
//    Tom Fogal, Thu Apr 30 12:08:07 MDT 2009
//    Do the right thing on Windows (untested..)
//
// ****************************************************************************
void
set(const char *k, const char *v)
{
#ifdef _WIN32
    if(SetEnvironmentVariable(k, v) == 0)
#else
    if(setenv(k, v, 1) != 0)
#endif
    {
        debug1 << "setenv(" << k << " = " << v << ") failed!" << std::endl
#ifdef _WIN32
               << "Error: " << GetLastError() << std::endl;
#else
               << "Error: " << errno << ": '" << strerror(errno) << std::endl;
#endif
    }
}

// ****************************************************************************
//  Function: Environment::unset
//
//  Purpose: Removes a variable definition from the environment.
//
//  Programmer: Tom Fogal
//
// ****************************************************************************
void
unset(const char *variable)
{
#ifdef HAVE_UNSETENV
# ifdef __APPLE__
    // Apple's unsetenv returns void; no error checking is possible.
    unsetenv(variable);
# else
    if(unsetenv(variable) != 0)
    {
        debug1 << "unsetenv(" << variable << ") failed!" << std::endl
               << "Error: " << errno << ": '" << strerror(errno) << std::endl;
    }
# endif
#else
    // level 5 because it doesn't usually matter if we clean up our environment
    // correctly; OS will do it when our process exits anyway.
    debug5 << "unsetenv(" << variable << ") ignored; unsetenv not supported "
           << "on this platform." << std::endl;
#endif
}

}  /* namespace Environment */
