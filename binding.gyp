{
	"targets": [
		{
			"target_name": "filesystem",
			"sources": [ 
				"filesystem.cpp",
        "tools.cpp",
        "access.cpp",
				"nannode.cpp"
			],
			"include_dirs": [
				"<!(node -e \"require('nan')\")"
			]
		}
	]
}
