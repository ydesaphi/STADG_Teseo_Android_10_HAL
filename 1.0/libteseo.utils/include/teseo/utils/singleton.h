/*
* This file is part of Teseo Android HAL
*
* Copyright (c) 2016-2018, STMicroelectronics - All Rights Reserved
* Author(s): Fabrice Deruy <fabrice.deruy@st.com> for STMicroelectronics.
*
* License terms: Apache 2.0.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/
#ifndef TESEO_HAL_UTILS_SINGLETON_H
#define TESEO_HAL_UTILS_SINGLETON_H

namespace stm {

template <typename T>
class Singleton
{
protected:
  // Constructor/destructor
  Singleton () { }
  ~Singleton () { }

public:
  // Interface 
  static T *getInstance ()
  {
    if (nullptr == _singleton)
      {
        _singleton = new T;
      }

    return (static_cast<T*> (_singleton));
  }

  static void kill ()
  {
    if (nullptr != _singleton)
      {
        delete _singleton;
        _singleton = nullptr;
      }
  }

private:
  // Unique instance
  static T *_singleton;
};

template <typename T>
T *Singleton<T>::_singleton = nullptr;

} //namespace stm
#endif