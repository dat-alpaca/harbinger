#pragma once
#include "core/assert/assert.hpp"

#include "core/assets/asset.hpp"

#include "core/config/config.hpp"

#include "core/containers/containers.hpp"
#include "core/containers/nano_free_list.hpp"
#include "core/containers/nano_string.hpp"
#include "core/containers/safe_containers.hpp"

#include "core/engine/engine.hpp"
#include "core/engine/context.hpp"

#include "core/event/event.hpp"
#include "core/event/event_type.hpp"
#include "core/event/concrete/asset_events.hpp"
#include "core/event/concrete/scene_events.hpp"
#include "core/event/concrete/window_events.hpp"

#include "core/logging/logger.hpp"

#include "core/memory/allocators.hpp"
#include "core/memory/dynamic_allocator.hpp"
#include "core/memory/linear_allocator.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/memory/memory_stats.hpp"

#include "core/scene/entity.hpp"
#include "core/scene/scene.hpp"

#include "core/utils/api_utils.hpp"
#include "core/utils/file.hpp"
#include "core/utils/json_serializable.hpp"
#include "core/utils/memory_utils.hpp"
#include "core/utils/string_utils.hpp"
#include "core/utils/time.hpp"
#include "core/utils/transform.hpp"

#include "core/defines.hpp"

