/**
 * @file Skills.cpp
 * @brief Skill registry implementation and built-in skill registration.
 *
 * Includes all skill definition headers and registers them at startup.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#include "Skills.h"
#include "Utils.h"
#include <string.h>

// ── Include all skill definitions ───────────────────────────────────────────
#include "skills/stand.h"
#include "skills/sit.h"
#include "skills/lay.h"
#include "skills/sleep.h"
#include "skills/walk.h"
#include "skills/back.h"
#include "skills/left.h"
#include "skills/right.h"
#include "skills/trot.h"
#include "skills/crawl.h"
#include "skills/wave.h"
#include "skills/shake.h"
#include "skills/stretch.h"

// ─────────────────────────────────────────────────────────────────────────────
// Construction
// ─────────────────────────────────────────────────────────────────────────────

SkillRegistry::SkillRegistry()
    : m_count(0)
{
    for (uint8_t i = 0; i < MAX_SKILLS; ++i) {
        m_skills[i].name   = nullptr;
        m_skills[i].motion = nullptr;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Built-in Skill Registration
// ─────────────────────────────────────────────────────────────────────────────

void SkillRegistry::registerBuiltins() {
    registerSkill("stand",   &Skills::standMotion);
    registerSkill("sit",     &Skills::sitMotion);
    registerSkill("lay",     &Skills::layMotion);
    registerSkill("sleep",   &Skills::sleepMotion);
    registerSkill("walk",    &Skills::walkMotion);
    registerSkill("back",    &Skills::backMotion);
    registerSkill("left",    &Skills::leftMotion);
    registerSkill("right",   &Skills::rightMotion);
    registerSkill("trot",    &Skills::trotMotion);
    registerSkill("crawl",   &Skills::crawlMotion);
    registerSkill("wave",    &Skills::waveMotion);
    registerSkill("shake",   &Skills::shakeMotion);
    registerSkill("stretch", &Skills::stretchMotion);

    DEBUG_PRINTF("[SkillRegistry] %d built-in skills registered.\n", m_count);
}

// ─────────────────────────────────────────────────────────────────────────────
// Registry Operations
// ─────────────────────────────────────────────────────────────────────────────

bool SkillRegistry::registerSkill(const char* name, const Motion* motion) {
    if (m_count >= MAX_SKILLS) {
        DEBUG_PRINTLN(F("[SkillRegistry] ERROR: Registry full."));
        return false;
    }
    if (!name || !motion) return false;

    m_skills[m_count].name   = name;
    m_skills[m_count].motion = motion;
    m_count++;

    return true;
}

const Motion* SkillRegistry::getSkill(const char* name) const {
    if (!name) return nullptr;

    for (uint8_t i = 0; i < m_count; ++i) {
        if (m_skills[i].name && strcmp(m_skills[i].name, name) == 0) {
            return m_skills[i].motion;
        }
    }

    DEBUG_PRINTF("[SkillRegistry] Skill not found: %s\n", name);
    return nullptr;
}

uint8_t SkillRegistry::getCount() const {
    return m_count;
}

void SkillRegistry::printSkills() const {
    Serial.printf("\n[SkillRegistry] %d skills registered:\n", m_count);
    for (uint8_t i = 0; i < m_count; ++i) {
        if (m_skills[i].name) {
            Serial.printf("  [%2d] %-10s  %2d keyframes  %s\n",
                          i, m_skills[i].name,
                          m_skills[i].motion ? m_skills[i].motion->numKeyframes : 0,
                          (m_skills[i].motion && m_skills[i].motion->loop) ? "loop" : "once");
        }
    }
    Serial.println();
}
