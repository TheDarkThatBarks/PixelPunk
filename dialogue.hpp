#pragma once

#include "json.hpp"
using json = nlohmann::json;
//using namespace nlohmann::literals;

const json dialogue = R"(
    {
        "&70": [
            {
                "player": false,
                "line": "Oh I didn't know anyone had survived that blast.50:Oh I didn't know anyone had survived that blast."
            }, {
                "player": false,
                "line": "What, your mouth doesn't work anymore?"
            }, {
                "player": true,
                "line": "Shut up."
            }
        ],
        "!f0": [
            {
                "r": 43,
                "c": 18,
                "lines": [
                    {
                        "player": false,
                        "line": "Oh I didn't know anyone had survived that blast."
                    }, {
                        "player": false,
                        "line": "We're pretty much all out at this point."
                    }, {
                        "player": false,
                        "line": "Head out through the vent. Watch out for the loose wires though."
                    }, {
                        "player": false,
                        "line": "..."
                    }, {
                        "player": false,
                        "line": "What, your mouth break in the explosion?"
                    }, {
                        "player": true,
                        "line": "Shut up."
                    }
                ]
            },
            {
                "r": 43,
                "c": 76,
                "lines": [
                    {
                        "player": false,
                        "line": "Watch out for those lasers. You definitely do not want to touch them."
                    },
                    {
                        "player": false,
                        "line": "I know from experience."
                    }
                ]
            }
        ]
    }
)"_json;