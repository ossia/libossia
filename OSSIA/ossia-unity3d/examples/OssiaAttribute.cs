using UnityEngine;
using System.Collections;
using Ossia;

public class OssiaAttribute : MonoBehaviour {

	// Attributes in libossia can be exposed this way : 
	[Ossia.Expose("sympa")]
	public int Sympa;

	// Use this for initialization
	void Start () {

	}

	// Update is called once per frame
	void Update () {

	}
}
