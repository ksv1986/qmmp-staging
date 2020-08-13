/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2014 Ricardo Villalba <rvm@users.sourceforge.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	Winfileassoc.cpp

	Handles file associations in Windows 7/Vista/XP/2000.
	We assume that the code is run without administrator privileges, so the associations are done for current user only.
	System-wide associations require writing to HKEY_CLASSES_ROOT and we don't want to get our hands dirty with that.
	Each user on the computer can configure his own set of file associations for SMPlayer, which is extremely cool.

	Optionally, during uninstall, it would be a good idea to call RestoreFileAssociations for all media types so
	that we can clean up the registry and restore the old associations for current user.

	Vista:
	The code can only register the app as default program for selected extensions and check if it is the default.
	It cannot restore 'old' default application, since this doesn't seem to be possible with the current Vista API.

	Add libole32.a library if compiling with MinGW. In smplayer.pro, under 'win32 {': LIBS += libole32

	Tested on: WinXP, Vista, Win7.

	Author: Florin Braghis (florin@libertv.ro)
*/

/*
   main changes compared to SMPlayer:
     replaced SMPlayer by Qmmp
     added icon cache update
     removed xp support
*/


#include "winfileassoc.h"
#include <QSettings>
#include <QApplication>
#include <QFileInfo>
#include <windows.h>
#include <shlobj.h>

#ifndef SHCNE_ASSOCHANGED
#define SHCNE_ASSOCHANGED __MSABI_LONG(0x08000000)
#endif

WinFileAssoc::WinFileAssoc(const QString AppName) : m_AppName(AppName)
{}

// Associates all extensions in the fileExtensions list with current app.
// Returns number of extensions processed successfully.
int WinFileAssoc::CreateFileAssociations(const QStringList &fileExtensions)
{
    return VistaSetAppsAsDefault(fileExtensions);
}

// Checks if extensions in extensionsToCheck are registered with this application. Returns a list of registered extensions.
// Returns false if there was an error accessing the registry.
// Returns true and 0 elements in registeredExtensions if no extension is associated with current app.
bool WinFileAssoc::GetRegisteredExtensions(const QStringList &extensionsToCheck, QStringList &registeredExtensions)
{
    registeredExtensions.clear();
    return VistaGetDefaultApps(extensionsToCheck, registeredExtensions);
}

// Windows Vista specific implementation

#if !defined(IApplicationAssociationRegistration)

typedef enum tagASSOCIATIONLEVEL {
    AL_MACHINE,
    AL_EFFECTIVE,
    AL_USER
} ASSOCIATIONLEVEL;

typedef enum tagASSOCIATIONTYPE {
    AT_FILEEXTENSION,
    AT_URLPROTOCOL,
    AT_STARTMENUCLIENT,
    AT_MIMETYPE
} ASSOCIATIONTYPE;

MIDL_INTERFACE("4e530b0a-e611-4c77-a3ac-9031d022281b")
IApplicationAssociationRegistration :
public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE QueryCurrentDefault(LPCWSTR pszQuery,
    ASSOCIATIONTYPE atQueryType,
    ASSOCIATIONLEVEL alQueryLevel,
    LPWSTR * ppszAssociation) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryAppIsDefault(LPCWSTR pszQuery,
    ASSOCIATIONTYPE atQueryType,
    ASSOCIATIONLEVEL alQueryLevel,
    LPCWSTR pszAppRegistryName,
    BOOL * pfDefault) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryAppIsDefaultAll(ASSOCIATIONLEVEL alQueryLevel,
    LPCWSTR pszAppRegistryName,
    BOOL * pfDefault) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetAppAsDefault(LPCWSTR pszAppRegistryName,
    LPCWSTR pszSet,
    ASSOCIATIONTYPE atSetType) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetAppAsDefaultAll(LPCWSTR pszAppRegistryName) = 0;
    virtual HRESULT STDMETHODCALLTYPE ClearUserAssociations(void) = 0;
};
#endif

static const CLSID CLSID_ApplicationAssociationReg = {0x591209C7, 0x767B, 0x42B2, {0x9F, 0xBA, 0x44, 0xEE, 0x46, 0x15, 0xF2, 0xC7}};
static const IID   IID_IApplicationAssociationReg  = {0x4e530b0a, 0xe611, 0x4c77, {0xa3, 0xac, 0x90, 0x31, 0xd0, 0x22, 0x28, 0x1b}};

int WinFileAssoc::VistaSetAppsAsDefault(const QStringList &fileExtensions)
{
    IApplicationAssociationRegistration *pAAR;
    HRESULT hr = CoCreateInstance(CLSID_ApplicationAssociationReg,
                                  NULL, CLSCTX_INPROC, IID_IApplicationAssociationReg,	(void **)&pAAR);

    int count = 0;

    if (SUCCEEDED(hr) && (pAAR != NULL)) {
        for(const QString &fileExtension : qAsConst(fileExtensions)) {
            hr = pAAR->SetAppAsDefault((const WCHAR *)m_AppName.utf16(),
                                       (const WCHAR *)QString("." + fileExtension).utf16(),
                                       AT_FILEEXTENSION);

            if (SUCCEEDED(hr))
                count++;
        }
        pAAR->Release();
    }

    return count;
}

bool WinFileAssoc::VistaGetDefaultApps(const QStringList &extensions, QStringList &registeredExt)
{
    IApplicationAssociationRegistration *pAAR;

    HRESULT hr = CoCreateInstance(CLSID_ApplicationAssociationReg,
                                  NULL, CLSCTX_INPROC, IID_IApplicationAssociationReg,	(void **)&pAAR);

    if (SUCCEEDED(hr) && (pAAR != NULL)) {
        for(const QString &fileExtension : qAsConst(extensions)) {
            BOOL bIsDefault = false;
            hr = pAAR->QueryAppIsDefault((const WCHAR *)QString("." + fileExtension).utf16(),
                                         AT_FILEEXTENSION,
                                         AL_EFFECTIVE,
                                         (const WCHAR *)m_AppName.utf16(),
                                         &bIsDefault);

            if (SUCCEEDED(hr) && bIsDefault) {
                registeredExt.append(fileExtension);
            }
        }

        pAAR->Release();
        return true;
    }

    return false;
}
