//         CHUNK                    , ID    , VER, NICE NAME                ) 

CHUNKDESC( DB_SNGLSOM               , 0x20  , 2  , "Geometry"               ) 
CHUNKDESC( DB_SNGLSOMBONE           , 0x21  , 3  , "Geometry"               ) 
CHUNKDESC( DB_SOMARRAY              , 0x22  , 2  , "Geometry"               ) 
CHUNKDESC( DB_SOMARRAYBONE          , 0x23  , 3  , "Geometry"               ) // for animated models with LOD (e.g. riders)
CHUNKDESC( DB_COLLISIONSOMARRAY     , 0x24  , 1  , "Collision"              ) // collision data for collision data generation by terraintoola and obbworld
CHUNKDESC( DB_AISCRIPT              , 0x25  , 2  , ""                       ) // ai scripts produced by animrip.exe
CHUNKDESC( DB_ANIMSCRIPT            , 0x26  , 2  , ""                       ) // animation scripts produced by animrip.exe
CHUNKDESC( DB_DMASNGLSOM            , 0x27  , 4  , "Geometry"               ) // Single SOM formated for PS2 DMA
CHUNKDESC( DB_DMASOMARRAY           , 0x28  , 4  , "Geometry"               ) // SOM array formated for PS2 DMA
CHUNKDESC( DB_SOMTEXTUREIDARRAY     , 0x29  , 1  , "Used texture IDs"       ) // Texture IDs used for SOMS
CHUNKDESC( DB_INSTANCE              , 0x60  , 6  , "Instances"              ) 
CHUNKDESC( DB_GROUP                 , 0x61  , 1  , ""                       ) 
CHUNKDESC( DB_LOD                   , 0x62  , 1  , "LODs"                   ) // level of detail records
CHUNKDESC( DB_EXPANSION_TABLE       , 0x63  , 1  , ""                       ) // patch bounds expansion tables
CHUNKDESC( DB_PARTICLE_EMITTER      , 0x64  , 6  , "Emitters"               ) // particle emitter source data
CHUNKDESC( DB_PARTICLE_SOURCE       , 0x65  , 8  , "Particles"              ) // source data for individual particles-- used to fill in ts_Particle structure
CHUNKDESC( DB_PARTICLE_EFFECT       , 0x66  , 2  , "Effects"                ) // an effect is made up of one or more channels, each channel can spawn one or more particles, sounds, or events
CHUNKDESC( DB_TEXTURE_CONTEXT       , 0x67  , 3  , "Texture contexts"       ) 
CHUNKDESC( DB_SCENE_TREE            , 0x68  , 1  , "Scene tree"             ) // a scene tree built out of CSceneNodes
CHUNKDESC( DB_VIS_REGIONS           , 0x69  , 3  , "Visibility regions"     ) // a list of visibility regions that enable/disable branches of the scene tree
CHUNKDESC( DB_SWOOSH_SOURCE         , 0x6a  , 1  , "Swooshes"               ) // source data for individual swooshes-- used to fill in ts_Swoosh structure
CHUNKDESC( DB_INSTANCE_GROUP        , 0x6b  , 1  , "Instance groups"        ) // a list of instances that make up a named group
CHUNKDESC( DB_SWOOSH_EMITTER        , 0x6c  , 3  , "Swoosh emitters"        ) // swoosh emitter source data
CHUNKDESC( DB_PALETTE_SET           , 0x81  , 1  , ""                       ) 
CHUNKDESC( DB_TEXTURE_SET           , 0x82  , 4  , "Textures"               ) 
CHUNKDESC( DB_TERRAIN_LIST          , 0x83  , 1  , "Terrain list"           ) // list of terrain types in force when .dbl was exported
CHUNKDESC( DB_EMITTER_LIST          , 0x84  , 1  , "Emitter names"          ) // list of particle emitter name strings
CHUNKDESC( DB_EFFECT_LIST           , 0x85  , 1  , "Effect names"           ) // list of particle effect name strings
CHUNKDESC( DB_MERGED_TEXTURE_SET    , 0x86  , 4  , "Textures"               ) // merged texture set for sprite/fluff driver
CHUNKDESC( DB_ANIMATED_TEXTURE_SET  , 0x87  , 2  , "Animated textures"      ) // animated texture set, all images same size and format
CHUNKDESC( DB_PARTICLE_LIST         , 0x88  , 1  , "Particle names"         ) // list of particle name strings
CHUNKDESC( DB_PARTICLE_SURFACE_LIST , 0x89  , 1  , "Particle surfaces"      ) // list of particle collision surface names
CHUNKDESC( DB_SWOOSH_LIST           , 0x8a  , 1  , "Swoosh names"           ) // list of swoosh name strings
CHUNKDESC( DB_SWOOSH_EMITTER_LIST   , 0x8b  , 1  , "Swoosh emitter names"   ) // list of swoosh emitter name strings
CHUNKDESC( DB_WORLD_EMITTERS        , 0x90  , 3  , "World emitters"         ) // particle emitters placed in world by artists
CHUNKDESC( DB_WORLD_DISTORTIONS     , 0x91  , 1  , "World distortions"      ) // distortions placed in world by artists
CHUNKDESC( DB_ANIM_CURVE            , 0x92  , 1  , "Animation curves"       ) // animation curve data for cut scenes
CHUNKDESC( DB_MOTION                , 0x100 , 6  , "Motion"                 ) // KFM motion data for boned models
CHUNKDESC( DB_BONEINFO              , 0x101 , 1  , "Bone info"              ) // bone information table: bone-to-gorup map and left-right bone name map
CHUNKDESC( DB_MOTION_NAMES          , 0x102 , 1  , "MotionNames"            ) // String table and index into the motion chunk
CHUNKDESC( DB_BONE_GROUP_MIRRORS    , 0x103 , 1  , ""                       ) // list of bone group mirrors
CHUNKDESC( DB_TEXTURE_NAMES         , 0x104 , 1  , "Texture Names"          ) // String table and index into the texture set
CHUNKDESC( DB_LIGHTS                , 0x200 , 2  , "Lights"                 ) // dynamic and/or fixed light records
CHUNKDESC( DB_NAMED_POINTS          , 0x201 , 1  , "Named points"           ) // name point records
CHUNKDESC( DB_SECTORS               , 0x202 , 1  , "Sectors"                ) // sector information
CHUNKDESC( DB_PATHS                 , 0x203 , 1  , "Paths"                  ) // path information
CHUNKDESC( DB_ATTACH_POINTS         , 0x204 , 1  , "Attach Points"          ) // attach point records
CHUNKDESC( DB_INSTANCE_ATTRIBUTES   , 0x209 , 1  , "Instance attributes"    ) // A bunch of parented key/value pairs for instances
CHUNKDESC( DB_CURVES                , 0x210 , 1  , ""                       ) // curve information (n degree bsplines)
CHUNKDESC( DB_CDB                   , 0x280 , 2  , "Collision geometry"     ) // Collision data base (terraintoola output)
CHUNKDESC( DB_ODB                   , 0x281 , 2  , "Collision OBBs"         ) // OBB data base (obbworld output)
CHUNKDESC( DB_PDB                   , 0x282 , 1  , "PDB"                    ) // Perimeter collision database (terraintoola output)
CHUNKDESC( DB_CPH                   , 0x283 , 1  , "CPH"                    ) // Collide partition header
CHUNKDESC( DB_NAV                   , 0x284 , 1  , "Navigation Mesh"        ) // Navigation mesh
CHUNKDESC( DB_XBOX_DXD              , 0x290 , 1  , "XBOX DXD"               ) // XBOX Resource (system memory part)
CHUNKDESC( DB_SOUNDBANK_3D          , 0x300 , 1  , ""                       ) // Soundbank that is really a hog file wrapped in a DBL chunk
CHUNKDESC( DB_SOUNDBANK_2D          , 0x301 , 1  , ""                       ) // Soundbank that is really a hog file wrapped in a DBL chunk
CHUNKDESC( DB_SOUNDBANK_MOD         , 0x302 , 1  , "Sound bank"             ) // soundmod.exe exported sound bank, wrapped in a DBL chunk
CHUNKDESC( DB_XML_SOURCE            , 0x400 , 1  , ""                       ) // XML source text data-- gets parsed into a tree, then discarded
CHUNKDESC( DB_OA_SOURCE             , 0x401 , -1 , ""                       ) // OA source text data-- gets parsed into a tree, then discarded
CHUNKDESC( DB_ENV_BINARY            , 0x402 , 1  , "Env. actor init"        ) // binary source for environmental actor initialization-- built from .oa or .xml data
CHUNKDESC( DB_CUT_BINARY            , 0x403 , 1  , "Cut scene binary data"  ) // binary source for cut scene manager
CHUNKDESC( DB_CONTEXT_BINARY        , 0x404 , 1  , "Context binary data"    ) // binary source for variable context/.var files
CHUNKDESC( DB_FILESTREAM            , 0x500 , 1  , "FileStream"             ) // file read via FileStream class
CHUNKDESC( DB_SOUNDSOURCE           , 0x600 , 1  , "Sound Source"           ) // Array of sound sources
CHUNKDESC( DB_SOUNDFIELD            , 0x601 , 1  , ""                       ) // Array of sound fields
CHUNKDESC( DB_WATER                 , 0x610 , 1  , "Water"                  ) // Array of water pools
CHUNKDESC( DB_GLASS                 , 0x620 , 1  , "Glass"                  ) // Array of Glass panes
CHUNKDESC( DB_HALO                  , 0x630 , 1  , ""                       ) // Array of Halos
CHUNKDESC( DB_STATIONARYCAM         , 0x631 , 1  , ""                       ) // Array of Stationary Cams
CHUNKDESC( DB_BLOOM                 , 0x632 , -1 , ""                       ) // Array of blooms
CHUNKDESC( DB_FILENAME_CHUNK        , 0x998 , 1  , "File name"              ) // chunk used in Uber dbl file processing
CHUNKDESC( DB_SKIP_CHUNK            , 0x999 , -1 , ""                       ) // chunk processing is skipped because user-supplied callback in dblLoad said so
