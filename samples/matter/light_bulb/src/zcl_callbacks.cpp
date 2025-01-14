/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <lib/support/logging/CHIPLogging.h>

#include "app_task.h"

#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/ConcreteAttributePath.h>

using namespace ::chip;
using namespace ::chip::app::Clusters;

void MatterPostAttributeChangeCallback(const chip::app::ConcreteAttributePath &attributePath, uint8_t type,
				       uint16_t size, uint8_t *value)
{
	ClusterId clusterId = attributePath.mClusterId;
	AttributeId attributeId = attributePath.mAttributeId;

	if (clusterId == OnOff::Id && attributeId == OnOff::Attributes::OnOff::Id) {
		ChipLogProgress(Zcl, "Cluster OnOff: attribute OnOff set to %" PRIu8 "", *value);
		AppTask::Instance().GetPWMDevice().InitiateAction(*value ? PWMDevice::ON_ACTION : PWMDevice::OFF_ACTION,
								  static_cast<int32_t>(AppEventType::Lighting), value);
	} else if (clusterId == LevelControl::Id && attributeId == LevelControl::Attributes::CurrentLevel::Id) {
		ChipLogProgress(Zcl, "Cluster LevelControl: attribute CurrentLevel set to %" PRIu8 "", *value);
		if (AppTask::Instance().GetPWMDevice().IsTurnedOn()) {
			AppTask::Instance().GetPWMDevice().InitiateAction(
				PWMDevice::LEVEL_ACTION, static_cast<int32_t>(AppEventType::Lighting), value);
		} else {
			ChipLogDetail(Zcl, "LED is off. Try to use move-to-level-with-on-off instead of move-to-level");
		}
	}
}

/** @brief OnOff Cluster Init
 *
 * This function is called when a specific cluster is initialized. It gives the
 * application an opportunity to take care of cluster initialization procedures.
 * It is called exactly once for each endpoint where cluster is present.
 *
 * @param endpoint   Ver.: always
 *
 * TODO Issue #3841
 * emberAfOnOffClusterInitCallback happens before the stack initialize the cluster
 * attributes to the default value.
 * The logic here expects something similar to the deprecated Plugins callback
 * emberAfPluginOnOffClusterServerPostInitCallback.
 *
 */
void emberAfOnOffClusterInitCallback(EndpointId endpoint)
{
	AppTask::Instance().UpdateClusterState();
}
