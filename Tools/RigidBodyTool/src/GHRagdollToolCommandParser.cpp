// Copyright 2010 Golden Hammer Software
#include "GHRagdollToolCommandParser.h"
#include "GHString.h"
#include "GHRagdollToolCommandHandler.h"
#include "GHPropertyContainer.h"
#include "GHRagdollToolProperties.h"

GHRagdollToolCommandParser::GHRagdollToolCommandParser(GHRagdollToolCommandHandler& handler,
                                                       const GHIdentifierMap<int>& enumStore,
                                                       GHPropertyContainer& props)
: mHandler(handler)
, mEnumStore(enumStore)
, mProps(props)
{
    
}

void GHRagdollToolCommandParser::parseCommand(const char* command)
{
    bool handled = false;
    
    struct CommandParse
    {
        typedef bool (GHRagdollToolCommandHandler::*ptrHandleCommand)(const char*);
        GHString mCommand;
        ptrHandleCommand mFunc;
        CommandParse(const char* command, ptrHandleCommand func)
        : mFunc(func)
        {
            mCommand.setConstChars(command, GHString::CHT_REFERENCE);
        }
    };
    
    CommandParse commandList[] =
    {
        CommandParse("shape=", &GHRagdollToolCommandHandler::setShape),
        CommandParse("height=", &GHRagdollToolCommandHandler::setHeight),
        CommandParse("width=", &GHRagdollToolCommandHandler::setWidth),
        CommandParse("depth=", &GHRagdollToolCommandHandler::setDepth),
        CommandParse("radius=", &GHRagdollToolCommandHandler::setRadius),
        CommandParse("limit=", &GHRagdollToolCommandHandler::setLimit),
        CommandParse("constrainttype=", &GHRagdollToolCommandHandler::setConstraintType),
        CommandParse("axis=", &GHRagdollToolCommandHandler::setAxis),
        CommandParse("playanim=", &GHRagdollToolCommandHandler::playAnim)
    };

    //This for loop may be confusing because of all the pointer indirection.
    // Essentially, we're looping through all of the commands that we support,
    // if our input command matches one of them, then we call the corresponding
    // function on our handler. We pass to the handler the rest of the string
    // that comes after the command string that we just used to determine
    // which function to call. This rest of the string contains any
    // parameters.
    for (int i = 0; i < sizeof(commandList)/sizeof(commandList[0])
                    && !handled; ++i)
    {
        CommandParse& cmd = commandList[i];
        if (!::strncmp(cmd.mCommand, command, cmd.mCommand.getCharLen()))
        {
            handled = (mHandler.*cmd.mFunc)(&command[cmd.mCommand.getCharLen()]);
        }
    }
    
    if (handled)
    {
        snprintf(mBuffer, BUFLEN, "Handled command: %s", command);
    }
    else
    {
        snprintf(mBuffer, BUFLEN, "Unable to handle command: %s", command);
    }
    mProps.setProperty(GHRagdollToolProperties::COMMANDRESULT, mBuffer);
}
