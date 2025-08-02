import bindings as kinoko

k = kinoko.KBindSystem.create_instance()
k.set_course(kinoko.Course.Mushroom_Gorge)
k.set_player(0, kinoko.Character.Funky_Kong, kinoko.Vehicle.Flame_Runner, False)
k.init()

controller = k.get_host_controller()

for _ in range(1000):
    controller.set_inputs(buttons=1, stick_x=0.0, stick_y=1.0, trick=kinoko.Trick.Neutral)
    k.step()

k.destroy_instance()