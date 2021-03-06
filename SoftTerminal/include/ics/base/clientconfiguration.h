/*
 * Copyright © 2016 Intel Corporation. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ICS_BASE_CLIENTCONFIGURATION_H_
#define ICS_BASE_CLIENTCONFIGURATION_H_

#include <vector>
#include <string>

#include "ics/base/commontypes.h"
#include "ics/base/network.h"

namespace ics {
namespace base{

/// Client configurations
struct ClientConfiguration {
  enum class CandidateNetworkPolicy : int { kAll = 1, kLowCost };

  ClientConfiguration()
       : candidate_network_policy(CandidateNetworkPolicy::kAll) {};

  /// List of ICE servers
  std::vector<IceServer> ice_servers;
  /**
   @brief Candidate collection policy.
   @details If you do not want cellular network when WiFi is available, please
   use CandidateNetworkPolicy::kLowCost. Using low cost policy may not have good
   network experience. Default policy is collecting all candidates.
   */
  CandidateNetworkPolicy candidate_network_policy;
};
}
}

#endif  // ICS_BASE_CLIENTCONFIGURATION_H_
