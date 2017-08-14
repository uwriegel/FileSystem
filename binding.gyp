{
	"targets": [
		{
			"target_name": "filesystem",
			"sources": [ 
				"filesystem.cpp",
				"tools.cpp",
				"nannode.cpp"
			],
			"include_dirs": [
				"<!(node -e \"require('nan')\")"
			]
		}
	]
}
