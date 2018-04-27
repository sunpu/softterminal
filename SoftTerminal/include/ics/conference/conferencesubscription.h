/*
 * Copyright © 2017 Intel Corporation. All Rights Reserved.
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

#ifndef ICS_CONFERENCE_SUBSCRIPTION_H_
#define ICS_CONFERENCE_SUBSCRIPTION_H_

#include <vector>
#include <mutex>

#include "ics/base/commontypes.h"
#include "ics/base/mediaconstraints.h"
#include "ics/base/subscription.h"
#include "ics/base/connectionstats.h"
#include "ics/base/exception.h"
#include "ics/conference/streamupdateobserver.h"
#include "ics/conference/subscribeoptions.h"

namespace rtc {
  class TaskQueue;
}

namespace webrtc{
  class StatsReport;
}

namespace ics {
namespace conference {

class ConferenceClient;

using namespace ics::base;

class ConferenceSubscription : public ConferenceStreamUpdateObserver,
                               public std::enable_shared_from_this<ConferenceSubscription> {
  public:
    ConferenceSubscription(std::shared_ptr<ConferenceClient> client, const std::string& sub_id,
                           const std::string& stream_id);

    virtual ~ConferenceSubscription();

    /// Pause current publication's audio or/and video basing on |track_kind| provided.
    void Mute(TrackKind track_kind,
              std::function<void()> on_success,
              std::function<void(std::unique_ptr<Exception>)> on_failure);
    /// Pause current publication's audio or/and video basing on |track_kind| provided.
    void Unmute(TrackKind track_kind,
                std::function<void()> on_success,
                std::function<void(std::unique_ptr<Exception>)> on_failure);
    /// Get conneciton stats of current publication
    void GetStats(
        std::function<void(std::shared_ptr<ConnectionStats>)> on_success,
        std::function<void(std::unique_ptr<Exception>)> on_failure);
    void GetNativeStats(
        std::function<void(
            const std::vector<const webrtc::StatsReport*>& reports)> on_success,
        std::function<void(std::unique_ptr<Exception>)> on_failure);
    /// Stop current publication.
    void Stop();
    /// If the Subscription is stopped or not.
    bool Ended() { return ended_; }
    /// Get the subscription ID
    std::string Id() const { return id_; }
	std::string StreamId() const { return stream_id_; }
    /// Update the subscription with new encoding settings.
    void ApplyOptions(
        const SubscriptionUpdateOptions& options,
        std::function<void()> on_success,
        std::function<void(std::unique_ptr<Exception>)> on_failure);
    /// Add observer on the subscription
    void AddObserver(SubscriptionObserver& observer);
    /// Remove observer on the subscription.
    void RemoveObserver(SubscriptionObserver& observer);
  private:
    void OnStreamMuteOrUnmute(const std::string& stream_id, TrackKind track_kind, bool muted);
    void OnStreamRemoved(const std::string& stream_id);
    std::string id_;
    std::string stream_id_;
    bool ended_;
    mutable std::mutex observer_mutex_;
    std::vector<std::reference_wrapper<SubscriptionObserver>> observers_;
    std::weak_ptr<ConferenceClient>  conference_client_;   // Weak ref to associated conference client
    std::shared_ptr<rtc::TaskQueue> event_queue_;
};


} // namespace conference
} // namespace ics

#endif  // ICS_CONFERENCE_SUBSCRIPTION_H_
