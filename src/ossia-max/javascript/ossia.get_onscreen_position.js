// autowatch = 1;
outlets = 1;

function screenposition(myObject) {
    var prev = this;
	var localposition = [this.patcher.getnamed(myObject).rect[0], this.patcher.getnamed(myObject).rect[1]];
    if (prev) {
		outlet(0, localposition[0] + prev.patcher.wind.location[0], localposition[1] + prev.patcher.wind.location[1]);
    }
}

function find(objName){
	outlet(0, "Im at" + this.patcher.getnamed(objName).rect);
	}

