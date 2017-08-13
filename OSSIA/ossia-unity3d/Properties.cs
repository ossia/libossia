namespace Ossia
{
	public enum ossia_type
	{
		FLOAT,
		INT,
		VEC2F,
		VEC3F,
		VEC4F,
		IMPULSE,
		BOOL,
		STRING,
		TUPLE,
		CHAR
	}

	public enum ossia_access_mode
	{
		BI,
		GET,
		SET
	}

	public enum ossia_bounding_mode
	{
		FREE,
		CLIP,
		WRAP,
		FOLD,
		LOW,
		HIGH
	}

	public delegate void ValueCallbackDelegate(Ossia.Value t);
}
