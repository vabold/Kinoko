#pragma once

namespace EGG {

// In the interest of building a complete representation of the library, we create an empty
// definition for ColorFader. This is meant to handle "calcCurrentFader" in SceneManager.
class ColorFader {
public:
    bool calc() {
        return false;
    }

    bool fadeIn() {
        return false;
    }

    bool fadeOut() {
        return false;
    }
};

} // namespace EGG
