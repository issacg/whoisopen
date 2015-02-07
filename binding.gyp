{
    "targets": [{
        "include_dirs" : [
            "<!(node -e \"require('nan')\")"
        ],
        "defines": [
            "PSAPI_VERSION=1"
        ],
        "libraries": ["-lPsapi"],
        "target_name": "whoisopen",
        "sources": ["src/whoisopen.cc"]
    }]
}