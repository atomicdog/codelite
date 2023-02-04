#if USE_SFTP
#include "ssh/cl_remote_executor.hpp"

#include "StringUtils.h"
#include "clModuleLogger.hpp"
#include "cl_standard_paths.h"
#include "processreaderthread.h"
#include "ssh/ssh_account_info.h"

#include <thread>
INITIALISE_SSH_LOG(LOG, "clRemoteExecutor");

clRemoteExecutor::clRemoteExecutor()
{
    Bind(wxEVT_SSH_CHANNEL_READ_ERROR, &clRemoteExecutor::OnChannelError, this);
    Bind(wxEVT_SSH_CHANNEL_WRITE_ERROR, &clRemoteExecutor::OnChannelError, this);
    Bind(wxEVT_SSH_CHANNEL_READ_OUTPUT, &clRemoteExecutor::OnChannelStdout, this);
    Bind(wxEVT_SSH_CHANNEL_READ_STDERR, &clRemoteExecutor::OnChannelStderr, this);
    Bind(wxEVT_SSH_CHANNEL_CLOSED, &clRemoteExecutor::OnChannelClosed, this);
}

clRemoteExecutor::~clRemoteExecutor()
{
    shutdown();
    Unbind(wxEVT_SSH_CHANNEL_READ_ERROR, &clRemoteExecutor::OnChannelError, this);
    Unbind(wxEVT_SSH_CHANNEL_WRITE_ERROR, &clRemoteExecutor::OnChannelError, this);
    Unbind(wxEVT_SSH_CHANNEL_READ_OUTPUT, &clRemoteExecutor::OnChannelStdout, this);
    Unbind(wxEVT_SSH_CHANNEL_READ_STDERR, &clRemoteExecutor::OnChannelStderr, this);
    Unbind(wxEVT_SSH_CHANNEL_CLOSED, &clRemoteExecutor::OnChannelClosed, this);
}

bool clRemoteExecutor::startup(const wxString& account_name)
{
    LOG_DEBUG(LOG) << "Initializing for account:" << account_name << endl;
    if(m_ssh) {
        LOG_WARNING(LOG) << "startup called for an already initialised session. account:" << account_name << endl;
        return true;
    }

    auto account = SSHAccountInfo::LoadAccount(account_name);
    if(account.GetHost().empty()) {
        LOG_WARNING(LOG) << "could not find account:" << account_name << endl;
        return false;
    }

    /// open channel
    try {
        m_ssh.reset(new clSSH(account.GetHost(), account.GetUsername(), account.GetPassword(), account.GetPort()));
        wxString message;

        m_ssh->Open();
        if(!m_ssh->AuthenticateServer(message)) {
            m_ssh->AcceptServerAuthentication();
        }
        m_ssh->Login();
    } catch(clException& e) {
        LOG_ERROR(LOG) << "Failed to open ssh channel to account:" << account.GetAccountName() << "." << e.What()
                       << endl;
        return false;
    }
    LOG_DEBUG(LOG) << "Initializing for account:" << account_name << "completed successfully" << endl;
    return true;
}

void clRemoteExecutor::shutdown() { m_ssh = nullptr; }

clSSHChannel* clRemoteExecutor::try_execute(const clRemoteExecutor::Cmd& cmd)
{
    if(!m_ssh) {
        LOG_WARNING(LOG) << "SSH session is not opened" << endl;
        return nullptr;
    }

    // open the channel
    clSSHChannel* channel = nullptr;
    try {
        channel = new clSSHChannel(m_ssh, clSSHChannel::kRemoteCommand, this, true);
        channel->Open();
    } catch(clException& e) {
        LOG_ERROR(LOG) << "failed to open channel." << e.What() << endl;
        wxDELETE(channel);
        return nullptr;
    }

    wxString command = ssh::build_command(cmd.command, cmd.wd, cmd.env);
    LOG_DEBUG(LOG) << "Executing command:" << command << endl;

    // prepare the commands

    try {
        channel->Execute(command);
        LOG_DEBUG(LOG) << "Success" << endl;
        return channel;
    } catch(clException& e) {
        LOG_TRACE(LOG) << "failed to execute remote command." << command << "." << e.What() << endl;
        wxDELETE(channel);
    }

    // the channel will delete itself upon completion
    return nullptr;
}

void clRemoteExecutor::OnChannelStdout(clCommandEvent& event)
{
    clProcessEvent output_event{ wxEVT_ASYNC_PROCESS_OUTPUT };
    output_event.SetStringRaw(event.GetStringRaw());
    LOG_DEBUG(LOG) << "stdout read:" << event.GetStringRaw().size() << "bytes" << endl;
    ProcessEvent(output_event);
}

void clRemoteExecutor::OnChannelStderr(clCommandEvent& event)
{
    clProcessEvent output_event{ wxEVT_ASYNC_PROCESS_STDERR };
    output_event.SetStringRaw(event.GetStringRaw());
    LOG_DEBUG(LOG) << "stderr read:" << event.GetStringRaw().size() << "bytes" << endl;
    ProcessEvent(output_event);
}

void clRemoteExecutor::OnChannelClosed(clCommandEvent& event)
{
    LOG_DEBUG(LOG) << "remote channel closed" << endl;

    clProcessEvent output_event{ wxEVT_ASYNC_PROCESS_TERMINATED };
    output_event.SetInt(event.GetInt());
    ProcessEvent(output_event);
}

void clRemoteExecutor::OnChannelError(clCommandEvent& event)
{
    wxUnusedVar(event);
    clProcessEvent command_ended{ wxEVT_ASYNC_PROCESS_TERMINATED };
    command_ended.SetInt(event.GetInt()); // exit code
    ProcessEvent(command_ended);
}

#endif // USE_SFTP
