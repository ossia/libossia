package io.ossia;
import com.sun.jna.Pointer;

public class InstanceBounds
{
    InstanceBounds(int pmin, int pmax)
    {
        min = pmin;
        max = pmax;
    }
    public int min = 0;
    public int max = 0;
};