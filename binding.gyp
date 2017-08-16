{
	"targets": [
		{
			"target_name": "filesystem",
			"sources": [ 
				"filesystem.cpp",
				"tools.cpp",
				"nantools.cpp",
				"access.cpp",
				"worker.cpp",
				"nannode.cpp"
			],
			"include_dirs": [
				"<!(node -e \"require('nan')\")"
			]
		}
	]
}
