/**
 * @file Skills.h
 * @brief Skill registry — maps skill names to Motion definitions.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#ifndef SKILLS_H
#define SKILLS_H

#include <Arduino.h>
#include "MotionEngine.h"

/** @brief Maximum number of skills that can be registered. */
static constexpr uint8_t MAX_SKILLS = 20;

/**
 * @brief A named entry in the skill registry.
 */
struct SkillEntry {
    const char*   name;     ///< Skill name (e.g., "stand")
    const Motion* motion;   ///< Pointer to the motion data
};

/**
 * @class SkillRegistry
 * @brief Central registry mapping skill names to Motion definitions.
 *
 * Skills are registered at startup and can be looked up by name
 * when a Bluetooth command or state change requests a specific skill.
 */
class SkillRegistry {
public:
    SkillRegistry();

    /**
     * @brief Register all built-in skills.
     *
     * Called during setup() to populate the registry with
     * all skill definitions from the skills/ directory.
     */
    void registerBuiltins();

    /**
     * @brief Register a custom skill.
     * @param name   Unique name for the skill.
     * @param motion Pointer to the Motion definition (must remain valid).
     * @return true if registered successfully.
     */
    bool registerSkill(const char* name, const Motion* motion);

    /**
     * @brief Look up a skill by name.
     * @param name The skill name to find.
     * @return Pointer to the Motion, or nullptr if not found.
     */
    const Motion* getSkill(const char* name) const;

    /**
     * @brief Get the number of registered skills.
     */
    uint8_t getCount() const;

    /**
     * @brief Print all registered skills to Serial.
     */
    void printSkills() const;

private:
    SkillEntry m_skills[MAX_SKILLS];
    uint8_t    m_count;
};

#endif // SKILLS_H
