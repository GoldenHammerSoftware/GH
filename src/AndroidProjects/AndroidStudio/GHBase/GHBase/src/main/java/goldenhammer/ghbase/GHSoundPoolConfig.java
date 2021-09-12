package goldenhammer.ghbase;

import java.util.HashMap;

//Map of filename to length. Can put other stuff here too if necessary.
//One of these per soundpool -- it is a list of multiple sound configs.
public class GHSoundPoolConfig
{
	public class Description
	{
		public Description(int length)
		{
			mLength = length;
		}
		public int mLength; //set to -1 if we don't need to store a length
	};
	private HashMap<String, Description> mDescriptions = new HashMap<String, Description>();
	
	public void addDescription(String filename, Description desc)
	{
		mDescriptions.put(filename, desc);
	}
	
	public int getLength(String filename)
	{
		Description desc = mDescriptions.get(filename);
		if (desc == null) {
			return -1;
		}
		return desc.mLength;
	}
}
