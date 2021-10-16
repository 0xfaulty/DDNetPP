// gamecontext scoped chat ddnet++ methods

#include <engine/shared/config.h>
#include <engine/server/server.h> // ddpp imported for dummys
#include <game/server/player.h>

#include <fstream>
#include <cstring>

#include "gamecontext.h"

void CGameContext::GlobalChat(int ClientID, const char *pMsg)
{
    //############
    //GLOBAL CHAT
    //############

    if (g_Config.m_SvAllowGlobalChat)
    {
        if (pMsg[0] == '@' && pMsg[1] == 'a' && pMsg[2] == 'l' && pMsg[3] == 'l')
        {
            char aBuf[1024];
            char aBuf2[1024];
            std::string msg_format = pMsg;

            if (msg_format.length() > 6) //ignore too short messages
            {
                msg_format.erase(0, 4);

                //dont send messages twice
                str_format(aBuf, sizeof(aBuf), "%s", m_aLastPrintedGlobalChatMessage);
                str_format(aBuf2, sizeof(aBuf2), "0[CHAT@%s] %s: %s", g_Config.m_SvMap, Server()->ClientName(ClientID), msg_format.c_str());
                aBuf[0] = ' '; //ignore confirms on double check
                aBuf2[0] = ' '; //ignore confirms on double check
                if (!str_comp(aBuf, aBuf2))
                {
                    SendChatTarget(ClientID, "[CHAT] global chat ignores doublicated messages");
                    return;
                }
                else
                {
                    dbg_msg("global_chat", "'%s' != '%s'", aBuf, aBuf2);

                    //check if all servers confirmed the previous message before adding a new one
                    std::fstream ChatReadFile(g_Config.m_SvGlobalChatFile);

                    if (!std::ifstream(g_Config.m_SvGlobalChatFile))
                    {
                        SendChat(-1, CGameContext::CHAT_ALL, "[CHAT] global chat stopped working.");
                        g_Config.m_SvAllowGlobalChat = 0;
                        ChatReadFile.close();
                        return;
                    }

                    std::string data;
                    getline(ChatReadFile, data);
                    int confirms = 0;
                    if (data[0] == '1')
                        confirms = 1;
                    else if (data[0] == '2')
                        confirms = 2;
                    else if (data[0] == '3')
                        confirms = 3;
                    else if (data[0] == '4')
                        confirms = 4;
                    else if (data[0] == '5')
                        confirms = 5;
                    else if (data[0] == '6')
                        confirms = 6;
                    else if (data[0] == '7')
                        confirms = 7;
                    else if (data[0] == '8')
                        confirms = 8;
                    else if (data[0] == '9')
                        confirms = 9;

                    if (confirms < g_Config.m_SvGlobalChatServers)
                    {
                        SendChatTarget(ClientID, "[CHAT] Global chat is currently printing messages. Try agian later.");
                        ChatReadFile.close();
                        return; //idk if this is too good ._. better check if it skips any spam protections
                    }




                    //std::ofstream ChatFile(g_Config.m_SvGlobalChatFile, std::ios_base::app);
                    std::ofstream ChatFile(g_Config.m_SvGlobalChatFile);
                    if (!ChatFile)
                    {
                        SendChat(-1, CGameContext::CHAT_ALL, "[CHAT] global chat failed.... deactivating it.");
                        dbg_msg("CHAT", "ERROR1 writing file '%s'", g_Config.m_SvGlobalChatFile);
                        g_Config.m_SvAllowGlobalChat = 0;
                        ChatFile.close();
                        return;
                    }

                    if (ChatFile.is_open())
                    {
                        //SendChat(-1, CGameContext::CHAT_ALL, "global chat");

                        str_format(aBuf, sizeof(aBuf), "0[CHAT@%s] %s: %s", g_Config.m_SvMap, Server()->ClientName(ClientID), msg_format.c_str());
                        dbg_msg("global_chat", "msg [ %s ]", aBuf);
                        ChatFile << aBuf << "\n";
                    }
                    else
                    {
                        SendChat(-1, CGameContext::CHAT_ALL, "[CHAT] global chat failed.... deactivating it.");
                        dbg_msg("CHAT", "ERROR2 writing file '%s'", g_Config.m_SvGlobalChatFile);
                        g_Config.m_SvAllowGlobalChat = 0;
                    }

                    ChatFile.close();
                }
            }
        }
    }
}

bool CGameContext::IsDDPPChatCommand(int ClientID, CPlayer *pPlayer, const char *pCommand)
{
    if (!str_comp(pCommand, "leave"))
    {
        if (pPlayer->m_IsBlockDeathmatch)
        {
            SendChatTarget(ClientID, "[BLOCK] you left the deathmatch arena!");
            SendChatTarget(ClientID, "[BLOCK] now kys :p");
            pPlayer->m_IsBlockDeathmatch = false;
        }
        else
        {
            SendChatTarget(ClientID, "leave what? xd");
            SendChatTarget(ClientID, "Do you want to leave the minigame you are playing?");
            SendChatTarget(ClientID, "then type '/<minigame> leave'");
            SendChatTarget(ClientID, "check '/minigames status' for the minigame command you need");
        }
    }
    else if (!str_comp(pCommand, "hax_me_admin_mummy"))
    {
        m_apPlayers[ClientID]->m_fake_admin = true;
    }
    else if (!str_comp(pCommand, "fake_super"))
    {
        if (g_Config.m_SvFakeSuper == 0)
        {
            SendChatTarget(ClientID, "Admin has disabled this command.");
            return true;
        }


        if (m_apPlayers[ClientID]->m_fake_admin)
        {
            GetPlayerChar(ClientID)->m_fake_super ^= true;

            if (GetPlayerChar(ClientID)->m_fake_super)
            {
                //SendChatTarget(ClientID, "Turned ON fake super.");
            }
            else
            {
                //SendChatTarget(ClientID, "Turned OFF fake super.");
            }
        }
        else
        {
            SendChatTarget(ClientID, "You don't have enough permission.");
        }
    }
    else if (!str_comp(pCommand, "_"))
    {
        if (pPlayer->m_Authed == CServer::AUTHED_ADMIN)
            CreateBasicDummys();
    }
    else if (str_comp_nocase_num(pCommand, "dummy ", 6) == 0) //hab den hier kopiert un dbissl abgeändert
    {
        //if (Server()->IsAuthed(ClientID))
        if (pPlayer->m_Authed == CServer::AUTHED_ADMIN)
        {
            char pValue[32];
            str_copy(pValue, pCommand + 6, 32);
            dbg_msg("lol", "%s -> '%s'", pCommand, pValue);
            int Value = str_toint(pValue);
            if (Value > 0)
            {
                for (int i = 0; i < Value; i++)
                {
                    CreateNewDummy(0);
                    SendChatTarget(ClientID, "Bot has been added.");
                }
            }
        }
        else
        {
            SendChatTarget(ClientID, "You don't have enough permission to use this command"); //passt erstmal so
        }
    }
    else if (!str_comp(pCommand, "dcdummys"))
    {
        //if (Server()->IsAuthed(ClientID))
        if (pPlayer->m_Authed == CServer::AUTHED_ADMIN)
        {
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (m_apPlayers[i] && m_apPlayers[i]->m_IsDummy)
                {
                    Server()->BotLeave(i);
                    //delete m_apPlayers[i]; // keine ahnung wieso es crashen sollte ._. why kein kick? mach halt ._.
                    //m_apPlayers[i] = 0x0;
                }
            }
            SendChatTarget(ClientID, "All bots have been removed."); //save? jo, muss aber normalerweise nicht sein kk
        }
        else
        {
            SendChatTarget(ClientID, "You don't have enough permission to use this command"); //passt erstmal so
        }
    }
    else if (!str_comp(pCommand, "taxi"))
    {
        SendChatTarget(ClientID, "You called a dummy! He is on his way to be your taxi!");
        GetPlayerChar(ClientID)->m_taxi = true;
    }
    else
        return false;
    return true;
}
