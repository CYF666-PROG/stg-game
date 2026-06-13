extends Label

# 配置参数
@export var target_scene: String = "res://scene/title.tscn" # 文字显现后要切换到的场景路径
@export var fade_speed: float = 0.5                             # 文字显现的速度（每秒增加的透明度）
@export var hold_time: float = 2.0                              # 文字完全显现后，在屏幕停留的时间（秒）

# 内部状态变量
var current_alpha: float = 0.0
var state: String = "fade_in" # 状态机：fade_in (淡入) -> hold (停留) -> switch (切换)
var timer: float = 0.0


func _ready() -> void:
	# 一进入场景，先把自己的透明度（Alpha）设为 0（完全透明）
	modulate.a = 0.0
	current_alpha = 0.0
	state = "fade_in"
	timer = 0.0


func _process(delta: float) -> void:
	match state:
		"fade_in":
			# 1. 每帧平滑增加透明度
			current_alpha += fade_speed * delta
			if current_alpha >= 1.0:
				current_alpha = 1.0
				state = "hold" # 变满了，进入下一个“停留”阶段
			
			# 将透明度应用到自身
			modulate.a = current_alpha
			
		"hold":
			# 2. 计时器累加，让文字在全显状态下挺留一会儿
			timer += delta
			if timer >= hold_time:
				state = "switch" # 停留时间够了，进入切换阶段
				
		"switch":
			# 3. 执行场景切换
			var error = get_tree().change_scene_to_file(target_scene)
			if error != OK:
				push_error("场景切换失败，请检查 target_scene 路径是否正确！")
			
			# 切换后关闭 process，防止在一帧内重复触发切换逻辑
			set_process(false)
