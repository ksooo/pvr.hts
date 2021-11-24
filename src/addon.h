/*
 *  Copyright (C) 2005-2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "kodi/AddonBase.h"

#include <memory>
#include <mutex>

namespace tvheadend
{
class Settings;
}

class ATTRIBUTE_HIDDEN CHTSAddon : public kodi::addon::CAddonBase
{
public:
  CHTSAddon();

  ADDON_STATUS Create() override;
  ADDON_STATUS SetSetting(const std::string& settingName,
                          const kodi::CSettingValue& settingValue) override;
  ADDON_STATUS CreateInstance(int instanceType,
                              const std::string& instanceID,
                              KODI_HANDLE instance,
                              const std::string& version,
                              KODI_HANDLE& addonInstance) override;

private:
  std::recursive_mutex m_mutex;
  std::shared_ptr<tvheadend::Settings> m_settings;
};
