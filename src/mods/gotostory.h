#pragma once

namespace gotostory {

enum class State {
    Default,
    LoadMenuReq,
    LoadStoryReq,
};

void tick();
void load_storymode();
State get_gotostory_state();

}  // namespace gotostory