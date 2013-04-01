
using System;
using System.Runtime.InteropServices;
using System.Security;

namespace FubiNET
{
    internal static class FubiInternal
    {
#if DEBUG
       private const string DLL_NAME = "Fubid.dll";
#else
       private const string DLL_NAME = "Fubi.dll";
#endif
       /**
	     * \brief Initializes Fubi with OpenNI using the given xml file and sets the skeleton profile.
	     *        If no xml file is given, Fubi will be intialized without OpenNI tracking enabled --> methods that need an openni context won't work.
	     * 
	     * @param openniXmlconfig name of the xml file for openNI initialization inlcuding all needed productions nodes 
		    (should be placed in the working directory, i.e. "bin" folder)
		    if config == 0x0, then OpenNI won't be initialized and Fubi stays in non-tracking mode
	     * @param profile set the openNI skeleton profile
	     * @param mirrorStream whether the stream should be mirrored or not
	     * @param smoothing set a smoothing factor for the tracking 0 = no smoothing to 0.99 strong smoothing
	     * @return true if succesfully initialized or already initialized before,
		    false means bad xml file or other serious problem with OpenNI initialization
	     *
	     * Default openni xml configuration file (note that only specific resolutions and FPS values are allowed):
	 	    <OpenNI>
		      <Log writeToConsole="true" writeToFile="false">
			    <!-- 0 - Verbose, 1 - Info, 2 - Warning, 3 - Error (default) -->
			    <LogLevel value="2"/>
		      </Log>
		      <ProductionNodes>
			    <Node type="Image">
			      <Configuration>
				    <MapOutputMode xRes="1280" yRes="1024" FPS="15"/>
				    <Mirror on="true"/>
			      </Configuration>
			    </Node>
			    <Node type="Depth">
			      <Configuration>
				    <MapOutputMode xRes="640" yRes="480" FPS="30"/>
				    <Mirror on="true"/>
			      </Configuration>
			    </Node>
			    <Node type="Scene" />
			    <Node type="User" />
		      </ProductionNodes>
		    </OpenNI>
	     */
        [DllImport(DLL_NAME, EntryPoint = "?init@Fubi@@YA_NPBDW4Profile@SkeletonTrackingProfile@1@_NM@Z"), SuppressUnmanagedCodeSecurity]
        [return: MarshalAs(UnmanagedType.U1)]
       internal static extern bool init(IntPtr openniXmlconfig, FubiUtils.SkeletonProfile profile, [MarshalAs(UnmanagedType.U1)]bool mirrorStream = true, float smoothing = 0);

        /**
	     * \brief Initializes Fubi with an specific options for the sensor init
	     * 
	     * @param depthWidth, ... options configuration for the sensor as in the SensorOptions struct
	     * @return true if succesfully initialized or already initialized before,
		    false means problem with sensor init
		    */
        [DllImport(DLL_NAME, EntryPoint = "?init@Fubi@@YA_NHHHHHHHHHW4Type@SensorType@1@W4Profile@SkeletonTrackingProfile@1@_NM@Z"), SuppressUnmanagedCodeSecurity]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool init(int depthWidth, int depthHeight, int depthFPS = 30,
		    int rgbWidth = 640, int rgbHeight = 480, int rgbFPS = 30,
		    int irWidth = -1, int irHeight = -1, int irFPS = -1,
            FubiUtils.SensorType type = FubiUtils.SensorType.OPENNI2,
		    FubiUtils.SkeletonProfile profile = FubiUtils.SkeletonProfile.ALL,
            [MarshalAs(UnmanagedType.U1)]bool mirrorStream = true, float smoothing = 0);

        /**
	        * \brief Allows you to switch between different sensor types during runtime
	        *		  Note that this will also reinitialize most parts of Fubi
	        * 
	        * @param options options for initializing the new sensor
	        * @return true if the sensor has been succesfully initialized
	        */
        [DllImport(DLL_NAME, EntryPoint = "?switchSensor@Fubi@@YA_NW4Type@SensorType@1@HHHHHHHHHW4Profile@SkeletonTrackingProfile@1@_NM@Z"), SuppressUnmanagedCodeSecurity]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool switchSensor(FubiUtils.SensorType type, int depthWidth, int depthHeight, int depthFPS = 30,
            int rgbWidth = 640, int rgbHeight = 480, int rgbFPS = 30,
            int irWidth = -1, int irHeight = -1, int irFPS = -1,
            FubiUtils.SkeletonProfile profile = FubiUtils.SkeletonProfile.ALL,
            [MarshalAs(UnmanagedType.U1)]bool mirrorStream = true, float smoothing = 0);

        /**
	     * \brief Get the currently available sensor types (defined in FubiConfig.h before compilation)
	     * 
	     * @return an int composed of the currently available sensor types (see SensorType enum for the meaning)
		    */
        [DllImport(DLL_NAME, EntryPoint = "?getAvailableSensorTypes@Fubi@@YAHXZ"), SuppressUnmanagedCodeSecurity]
        internal static extern int getAvailableSensorTypes();

        /**
	     * \brief Get the type of the currently active sensor
	     * 
	     * @return the current sensor type
		    */
         [DllImport(DLL_NAME, EntryPoint = "?getCurrentSensorType@Fubi@@YA?AW4Type@SensorType@1@XZ"), SuppressUnmanagedCodeSecurity]
        internal static extern FubiUtils.SensorType getCurrentSensorType();


        /**
         * \brief Shuts down OpenNI and the tracker, releasing all allocated memory
         * 
         */
        [DllImport(DLL_NAME, EntryPoint = "?release@Fubi@@YAXXZ"), SuppressUnmanagedCodeSecurity]
        internal static extern void release();

        /**
          * \brief Updates the sensor to get the next frame of depth, rgb, and tracking data.
          *        Also searches for users in the scene and loads the default tracking calibration for new users or request a calibration
          * 
          */
        [DllImport(DLL_NAME, EntryPoint = "?updateSensor@Fubi@@YAXXZ"), SuppressUnmanagedCodeSecurity]
        internal static extern void updateSensor();

        /**
          * \brief retrieve an image from one of the OpenNI production nodes with specific format and optionally enhanced by different
	      *        tracking information 
	      *		  Some render options require an OpenCV installation!
	      *
	      * @param outputImage pointer to an unsigned char array
	      *        Will be filled with wanted image
	      *		  Array has to be of correct size, e.g. depth image (640x480 std resolution) with tracking info
	      *		  requires 4 channels (RGBA) --> size = 640*480*4 = 1228800
	      * @param type can be color, depth, or ir image
	      * @param numChannels number channels in the image 1, 3 or 4
	      * @param depth the pixel depth of the image, 8 bit (standard) or 16 bit (mainly usefull for depth images
	      * @param renderOptions options for rendering additional informations into the image (e.g. tracking skeleton)
	      * @param depthModifications options for transforming the depht image to a more visible format
          * @param userId If set to something else than 0 an image will be cut cropped around (the joint of interest of) this user, if 0 the whole image is put out.
	      * @param jointOfInterest the joint of the user the image is cropped around and a threshold on the depth values is applied.
			  If set to num_joints fubi tries to crop out the whole user.
         */
        [DllImport(DLL_NAME, EntryPoint = "?getImage@Fubi@@YA_NPAEW4Type@ImageType@1@W4Channel@ImageNumChannels@1@W4Depth@ImageDepth@1@IW4Modification@DepthImageModification@1@IW4Joint@SkeletonJoint@1@@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern void getImage(IntPtr outputImage, FubiUtils.ImageType type, FubiUtils.ImageNumChannels numChannels, FubiUtils.ImageDepth depth, 
		uint renderOptions = (uint)FubiUtils.RenderOptions.Default,
        FubiUtils.DepthImageModification depthModifications = FubiUtils.DepthImageModification.UseHistogram,
            UInt32 userId = 0, FubiUtils.SkeletonJoint jointOfInterest = FubiUtils.SkeletonJoint.NUM_JOINTS);


        /**
	     * \brief Tries to recognize a posture in the current frame of tracking data of one user
	     * 
	     * @param postureID enum id of the posture to be found in FubiPredefinedGestures.h
	     * @param userID the OpenNI user id of the user to be checked
	     * @return RECOGNIZED in case of a succesful detection, TRACKING_ERROR if a needed joint is currently not tracked, NOT_RECOGNIZED else
	     */
        [DllImport(DLL_NAME, EntryPoint = "?recognizeGestureOn@Fubi@@YA?AW4Result@RecognitionResult@1@W4Posture@Postures@1@I@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern FubiUtils.RecognitionResult recognizeGestureOn(FubiPredefinedGestures.Postures postureID, UInt32 userID);

        /**
         * \brief Checks a user defined gesture or posture recognizer for its success
         * 
         * @param recognizerID id of the recognizer return during its creation
         * @param userID the OpenNI user id of the user to be checked
         * @return true in case of a succesful detection
         */
        [DllImport(DLL_NAME, EntryPoint = "?recognizeGestureOn@Fubi@@YA?AW4Result@RecognitionResult@1@II@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern FubiUtils.RecognitionResult recognizeGestureOn(UInt32 recognizerIndex, UInt32 userID);

        /**
         * \brief Returns the OpenNI user id from the user index
         * 
         * @param index index of the user in the user array
         * @return OpenNI user id of that user or 0 if not found
         */
        [DllImport(DLL_NAME, EntryPoint = "?getUserID@Fubi@@YAII@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern UInt32 getUserID(UInt32 index);

        /**
	     * \brief Automatically starts combination recogntion for new users
	     * 
	     * @param enable if set to true, the recognizer will automatically start for new users, else this must be done manually (by using enableCombinationRecognition(..))
	     * @param combinationID enum id of the combination to be found in FubiPredefinedGestures.h or NUM_COMBINATIONS for all combinations
	     */
        [DllImport(DLL_NAME, EntryPoint = "?setAutoStartCombinationRecognition@Fubi@@YAX_NW4Combination@Combinations@1@@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern void setAutoStartCombinationRecognition([MarshalAs(UnmanagedType.U1)]bool enable, FubiPredefinedGestures.Combinations combinationID = FubiPredefinedGestures.Combinations.NUM_COMBINATIONS);


        /**
	     * \brief Check if autostart is activated for a combination recognizer
	     * 
	     * @param combinationID enum id of the combination to be found in FubiPredefinedGestures.h or NUM_COMBINATIONS for all combinations
	     * @return true if the corresponding auto start is activated
	     */
        [DllImport(DLL_NAME, EntryPoint = "?getAutoStartCombinationRecognition@Fubi@@YA_NW4Combination@Combinations@1@@Z"), SuppressUnmanagedCodeSecurity]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool getAutoStartCombinationRecognition(FubiPredefinedGestures.Combinations combinationID = FubiPredefinedGestures.Combinations.NUM_COMBINATIONS);

        /**
         * \brief Starts or stops the recognition process of a combination for one user
         * 
         * @param combinationID enum id of the combination to be found in FubiPredefinedGestures.h
         * @param userID the OpenNI user id of the user for whom the recognizers should be modified
         * @param enable if set to true, the recognizer will be started (if not already stared), else it stops
         */
        [DllImport(DLL_NAME, EntryPoint = "?enableCombinationRecognition@Fubi@@YAXW4Combination@Combinations@1@I_N@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern void enableCombinationRecognition(FubiPredefinedGestures.Combinations combinationID, UInt32 userID, [MarshalAs(UnmanagedType.U1)]bool enable);

        /**
         * \brief Checks a combination recognizer for its success
         * 
         * @param combinationID  enum id of the combination to be found in FubiPredefinedGestures.h
         * @param userID the OpenNI user id of the user to be checked
         * @param userStates (= 0x0) pointer to a vector of tracking data that represents the tracking information of the user
         *		  during the recognition of each state
         * @param restart (=true) if set to true, the recognizer automatically restarts, so the combination can be recognized again.
         * @return true in case of a succesful detection
         */
        [DllImport(DLL_NAME, EntryPoint = "?getCombinationRecognitionProgressOn@Fubi@@YA?AW4Result@RecognitionResult@1@W4Combination@Combinations@1@IPAV?$vector@UTrackingData@FubiUser@@V?$allocator@UTrackingData@FubiUser@@@std@@@std@@_N@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern FubiUtils.RecognitionResult getCombinationRecognitionProgressOn(FubiPredefinedGestures.Combinations combinationID, UInt32 userID, IntPtr userStates, [MarshalAs(UnmanagedType.U1)]bool restart = true);

        /**
         * \brief Returns true if OpenNI has been already initialized
         * 
         */
        [DllImport(DLL_NAME, EntryPoint = "?isInitialized@Fubi@@YA_NXZ"), SuppressUnmanagedCodeSecurity]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool isInitialized();

        /**
        * \brief Returns the current depth resolution
        * 
        * @param width, height the resolution
        */
        [DllImport(DLL_NAME, EntryPoint = "?getDepthResolution@Fubi@@YAXAAH0@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern void getDepthResolution(out Int32 width, out Int32 height);

        /**
         * \brief Returns the current rgb resolution
         * 
         * @param width, height the resolution
         */
        [DllImport(DLL_NAME, EntryPoint = "?getRgbResolution@Fubi@@YAXAAH0@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern void getRgbResolution(out Int32 width, out Int32 height);

        /**
         * \brief Returns the current rgb resolution
         * 
         * @param width, height the resolution
         */
        [DllImport(DLL_NAME, EntryPoint = "?getIRResolution@Fubi@@YAXAAH0@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern void getIRResolution(out Int32 width, out Int32 height);


         /**
		 * \brief Loads a recognizer config xml file and adds the configured recognizers
		 * 
		 * @para fileName name of the xml config file
		 * @return true if at least one recognizers was loaded from the given xml
		 */
        [DllImport(DLL_NAME, EntryPoint = "?loadRecognizersFromXML@Fubi@@YA_NPBD@Z"), SuppressUnmanagedCodeSecurity]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool loadRecognizersFromXML(IntPtr fileName);

        /**
         * \brief Returns current number of user defined recognizers
         * 
         * @return number of recognizers, the recognizers also have the indices 0 to numberOfRecs-1
         */
        [DllImport(DLL_NAME, EntryPoint = "?getNumUserDefinedRecognizers@Fubi@@YAIXZ"), SuppressUnmanagedCodeSecurity]
        internal static extern UInt32 getNumUserDefinedRecognizers();

        /**
	     * \brief Returns the name of a user defined recognizer
	     * 
	     * @param  recognizerIndex index of the recognizer
	     * @return returns the recognizer name or an empty string if the user is not found or the name not set
	     */
        [DllImport(DLL_NAME, EntryPoint = "?getUserDefinedRecognizerName@Fubi@@YAPBDI@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern IntPtr getUserDefinedRecognizerName(UInt32 recognizerIndex);

        /**
         * \brief Returns the index of a user defined recognizer
         * 
         * @param recognizerName name of the recognizer
         * @return returns the recognizer name or -1 if not found
         */
        [DllImport(DLL_NAME, EntryPoint = "?getUserDefinedRecognizerIndex@Fubi@@YAHPBD@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern UInt32 getUserDefinedRecognizerIndex(IntPtr recognizerName);

        /**
	     * \brief Checks a user defined gesture or posture recognizer for its success
	     * 
	     * @param recognizerName name of the recognizer return during its creation
	     * @param userID the OpenNI user id of the user to be checked
	     * @return true in case of a succesful detection
	     */
        [DllImport(DLL_NAME, EntryPoint = "?recognizeGestureOn@Fubi@@YA?AW4Result@RecognitionResult@1@PBDI@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern FubiUtils.RecognitionResult recognizeGestureOn(IntPtr recognizerName, UInt32 userID);

        /**
	     * \brief Returns the number of shown fingers detected at the hand of one user (REQUIRES OPENCV!)
	     * 
	     * @param userID OpenNI id of the user
	     * @param leftHand looks at the left instead of the right hand
	     * @param getMedianOfLastFrames uses the precalculated median of finger counts of the last frames (still calculates new one if there is no precalculation)
	     * @param useConvexityDefectMethod if true using old method that calculates the convexity defects
	     * @return the number of shown fingers detected, 0 if there are none or there is an error
	     */
        [DllImport(DLL_NAME, EntryPoint = "?getFingerCount@Fubi@@YAHI_N00@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern Int32 getFingerCount(UInt32 userID, [MarshalAs(UnmanagedType.U1)]bool leftHand = false, [MarshalAs(UnmanagedType.U1)]bool getMedianOfLastFrames = true, [MarshalAs(UnmanagedType.U1)]bool useConvexityDefectMethod = false);

        /**
	     * \brief Checks a user defined combination recognizer for its success
	     * 
	     * @param recognizerName name of the combination
	     * @param userID the OpenNI user id of the user to be checked
	     * @param userStates (= 0x0) pointer to a vector of tracking data that represents the tracking information of the user
	     *		  during the recognition of each state
	     * @param restart (=true) if set to true, the recognizer automatically restarts, so the combination can be recognized again.
	     * @return true in case of a succesful detection
	     */
        [DllImport(DLL_NAME, EntryPoint = "?getCombinationRecognitionProgressOn@Fubi@@YA?AW4Result@RecognitionResult@1@PBDIPAV?$vector@UTrackingData@FubiUser@@V?$allocator@UTrackingData@FubiUser@@@std@@@std@@_N@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern FubiUtils.RecognitionResult getCombinationRecognitionProgressOn(IntPtr recognizerName, UInt32 userID, IntPtr userStates, [MarshalAs(UnmanagedType.U1)]bool restart = true);

        /**
	     * \brief Returns the index of a user defined combination recognizer
	     * 
	     * @param recognizerName name of the recognizer
	     * @return returns the recognizer name or -1 if not found
	     */
        [DllImport(DLL_NAME, EntryPoint = "?getUserDefinedCombinationRecognizerIndex@Fubi@@YAHPBD@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern Int32 getUserDefinedCombinationRecognizerIndex(IntPtr recognizerName);

	    /**
	     * \brief Returns current number of user defined combination recognizers
	     * @return number of recognizers, the recognizers also have the indices 0 to numberOfRecs-1
	     */
        [DllImport(DLL_NAME, EntryPoint = "?getNumUserDefinedCombinationRecognizers@Fubi@@YAIXZ"), SuppressUnmanagedCodeSecurity]
        internal static extern UInt32 getNumUserDefinedCombinationRecognizers();

	    /**
	     * \brief Returns the name of a user defined combination recognizer
	     * 
	     * @param  recognizerIndex index of the recognizer
	     * @return returns the recognizer name or an empty string if the user is not found or the name not set
	     */
        [DllImport(DLL_NAME, EntryPoint = "?getUserDefinedCombinationRecognizerName@Fubi@@YAPBDI@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern IntPtr getUserDefinedCombinationRecognizerName(UInt32 recognizerIndex);

        /**
	     * \brief Starts or stops the recognition process of a user defined combination for one user
	     * 
	     * @param combinationName name defined for this recognizer
	     * @param userID the OpenNI user id of the user for whom the recognizers should be modified
	     * @param enable if set to true, the recognizer will be started (if not already stared), else it stops
	     */
        [DllImport(DLL_NAME, EntryPoint = "?enableCombinationRecognition@Fubi@@YAXPBDI_N@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern void enableCombinationRecognition(IntPtr combinationName, UInt32 userID, [MarshalAs(UnmanagedType.U1)]bool enable);

        /**
         * \brief Returns the color for a user in the background image
         * 
         * @param id OpennNI user id of the user of interest
         * @param r, g, b returns the red, green, and blue components of the color in which the users shape is displayed in the tracking image
         *        returns 0,0,0 (black) if not found
         */
        [DllImport(DLL_NAME, EntryPoint = "?getColorForUserID@Fubi@@YAXIAAM00@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern void getColorForUserID(UInt32 id, out float r, out float g, out float b);

        /**
	     * \brief save an image from one of the OpenNI production nodes with specific format and optionally enhanced by different
	     *        tracking information
	     *
	     * @param filename filename where the image should be saved to
	     *        can be relative to the working directory (bin folder) or absolute
	     *		  the file extension determins the file format (should be jpg)
	     * @param jpegQuality qualitiy (= 88) of the jpeg compression if a jpg file is requested, ranges from 0 to 100 (best quality)
	     * @param type can be color, depth, or ir image
	     * @param numChannels number channels in the image 1, 3 or 4
	     * @param depth the pixel depth of the image, 8 bit (standard) or 16 bit (mainly usefull for depth images
	     * @param renderOptions options for rendering additional informations into the image (e.g. tracking skeleton)
	     * @param depthModifications options for transforming the depht image to a more visible format
         * @param userId If set to something else than 0 an image will be cut cropped around (the joint of interest of) this user, if 0 the whole image is put out.
	     * @param jointOfInterest the joint of the user the image is cropped around and a threshold on the depth values is applied.
			      If set to num_joints fubi tries to crop out the whole user.
	    */

        [DllImport(DLL_NAME, EntryPoint = "?saveImage@Fubi@@YA_NPBDHW4Type@ImageType@1@W4Channel@ImageNumChannels@1@W4Depth@ImageDepth@1@IW4Modification@DepthImageModification@1@IW4Joint@SkeletonJoint@1@@Z"), SuppressUnmanagedCodeSecurity]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool saveImage(IntPtr fileName, int jpegQuality /*0-100*/,
            FubiUtils.ImageType type, FubiUtils.ImageNumChannels numChannels, FubiUtils.ImageDepth depth,
            uint renderOptions = (uint)FubiUtils.RenderOptions.Default,
            FubiUtils.DepthImageModification depthModifications = FubiUtils.DepthImageModification.UseHistogram,
            UInt32 userId = 0, FubiUtils.SkeletonJoint jointOfInterest = FubiUtils.SkeletonJoint.NUM_JOINTS);

        /**
	     * \brief Creates a user defined posture recognizer
	     * 
	     * @param joint the joint of interest
	     * @param relJoint the joint in which it has to be in a specifc relation
	     * @param minValues (=-inf, -inf, -inf) the minimal values allowed for the vectore relJoint -> joint
	     * @param maxValues (=inf, inf, inf) the maximal values allowed for the vectore relJoint -> joint
	     * @param minDistance (= 0) the minimal distance between joint and relJoint
	     * @param minDistance (= inf) the maximal distance between joint and relJoint
         * @param localPosition if set to true, the function will return local position (vector from parent joint)
	     * @param atIndex (= -1) if an index is given, the corresponding recognizer will be replaced instead of creating a new one
	     *
	     * @return index of the recognizer needed to call it later
	     */
        [DllImport(DLL_NAME, EntryPoint = "?addJointRelationRecognizer@Fubi@@YAIW4Joint@SkeletonJoint@1@0MMMMMMMM_NHPBDMW4Measurement@BodyMeasurement@1@@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern UInt32 addJointRelationRecognizer(FubiUtils.SkeletonJoint joint, FubiUtils.SkeletonJoint relJoint,
		    float minX, float minY, float minZ, 
		    float maxX, float maxY, float maxZ, 
		    float minDistance, float maxDistance, 
            [MarshalAs(UnmanagedType.U1)] bool useLocalPositions,
            Int32 atIndex, IntPtr name,
            float minConfidence, FubiUtils.BodyMeasurement measuringUnit);

        /**
	     * \brief Creates a user defined linear movement recognizer
	     * 
	     * @param joint the joint of interest
	     * @param relJoint the joint in which it has to be in a specifc relation
	     * @param direction the direction in which the movement should happen
	     * @param minVel the minimal velocity that has to be reached in this direction
	     * @param maxVel (= inf) the maximal velocity that is allowed in this direction
	     * @param atIndex (= -1) if an index is given, the corresponding recognizer will be replaced instead of creating a new one
	     * @param name name of the recognizer
	     * @param maxAngleDifference (=45°) the maximum angle difference that is allowed between the requested direction and the actual movement direction
	     * @param bool useOnlyCorrectDirectionComponent (=true) If true, this only takes the component of the actual movement that is conform
	     *				the requested direction, else it always uses the actual movement for speed calculation
	     *
	     * @return index of the recognizer needed to call it later
	     */
          // A linear gesture has a vector calculated as joint - relative joint, 
          // the direction (each component -1 to +1) that will be applied per component on the vector, and a min and max vel in milimeter per second
        [DllImport(DLL_NAME, EntryPoint = "?addLinearMovementRecognizer@Fubi@@YAIW4Joint@SkeletonJoint@1@MMMMM_NHPBDM1@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern UInt32 addLinearMovementRecognizer(FubiUtils.SkeletonJoint joint,
		    float dirX, float dirY, float dirZ, float minVel, float maxVel,
            [MarshalAs(UnmanagedType.U1)] bool useLocalPositions,
            int atIndex, IntPtr name, float maxAngleDifference, [MarshalAs(UnmanagedType.U1)] bool useOnlyCorrectDirectionComponent);
        [DllImport(DLL_NAME, EntryPoint = "?addLinearMovementRecognizer@Fubi@@YAIW4Joint@SkeletonJoint@1@0MMMMM_NHPBDM1@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern UInt32 addLinearMovementRecognizer(FubiUtils.SkeletonJoint joint, FubiUtils.SkeletonJoint relJoint, 
		    float dirX, float dirY, float dirZ, float minVel, float maxVel,
            [MarshalAs(UnmanagedType.U1)] bool useLocalPositions,
            int atIndex, IntPtr name, float maxAngleDifference, [MarshalAs(UnmanagedType.U1)] bool useOnlyCorrectDirectionComponent);

        /**
	     * \brief Creates a user defined finger count recognizer
	     * 
	     * @param joint the hand joint of interest
	     * @param minFingers the minimum number of fingers the user should show up
	     * @param maxFingers the maximum number of fingers the user should show up
	     * @param atIndex (= -1) if an index is given, the corresponding recognizer will be replaced instead of creating a new one
	     * @param name (= 0) sets a name for the recognizer (should be unique!)
	     * @param minConfidence (=-1) if given this is the mimimum confidence required from tracking for the recognition to be succesful
	     * @param useMedianCalculation (=false) if true, the median for the finger count will be calculated over several frames instead of always taking the current detection
	     *
	     * @return index of the recognizer needed to call it later
	     */
         [DllImport(DLL_NAME, EntryPoint = "?addFingerCountRecognizer@Fubi@@YAIW4Joint@SkeletonJoint@1@IIHPBDM_N@Z"), SuppressUnmanagedCodeSecurity]
         internal static extern UInt32 addFingerCountRecognizer(FubiUtils.SkeletonJoint handJoint,
		    UInt32 minFingers, UInt32 maxFingers,
		    Int32 atIndex,
		    IntPtr name,
		    float minConfidence,
		    bool useMedianCalculation);

        /**
	     * \brief Creates a user defined joint orientation recognizer
	     * 
	     * @param joint the joint of interest
	     * @param minValues (=-180, -180, -180) the minimal degrees allowed for the joint orientation
	     * @param maxValues (=180, 180, 180) the maximal degrees allowed for the joint orientation
	     * @param useLocalOrientations if true, uses a local orienation in which the parent orientation has been substracted
	     * @param atIndex (= -1) if an index is given, the corresponding recognizer will be replaced instead of creating a new one
	     * @param name (= 0) sets a name for the recognizer (should be unique!)
	     * @param minConfidence (=-1) if given this is the mimimum confidence required from tracking for the recognition to be succesful
	     *
	     * @return index of the recognizer needed to call it later
	     */
	    [DllImport(DLL_NAME, EntryPoint = "?addJointOrientationRecognizer@Fubi@@YAIW4Joint@SkeletonJoint@1@MMMMMM_NHPBDM@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern UInt32 addJointOrientationRecognizer(FubiUtils.SkeletonJoint joint,
		    float minX , float minY , float minZ,
		    float maxX, float maxY, float maxZ,
		    bool useLocalOrientations,
		    int atIndex,
		    IntPtr name,
		    float minConfidence);

        /**
	     * \brief load a combination recognizer from a string that represents an xml node with the combination definition
	     * 
	     * @para xmlDefinition string containing the xml definition
	     * @return true if the combination was loaded succesfully
	     */ 
        [DllImport(DLL_NAME, EntryPoint = "?addCombinationRecognizer@Fubi@@YA_NPBD@Z"), SuppressUnmanagedCodeSecurity]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool addCombinationRecognizer(IntPtr xmlDefinition);
      	

        /**
	     * \brief  Whether the user is currently seen in the depth image
	     *
	     * @param userID OpenNI id of the user
	     */
        [DllImport(DLL_NAME, EntryPoint = "?isUserInScene@Fubi@@YA_NI@Z"), SuppressUnmanagedCodeSecurity]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool isUserInScene(UInt32 userID);

        /**
	     * \brief Whether the user is currently tracked
	     *
	     * @param userID OpenNI id of the user
	     */
        [DllImport(DLL_NAME, EntryPoint = "?isUserTracked@Fubi@@YA_NI@Z"), SuppressUnmanagedCodeSecurity]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool isUserTracked(UInt32 userID);

        /**
	     * \brief Get the most current tracking info of the user
	     * (including all joint positions and orientations, the center of mass and a timestamp)
	     *
	     * @param userID OpenNI id of the user
	     * @return the user tracking info struct
	     */
        [DllImport(DLL_NAME, EntryPoint = "?getCurrentTrackingData@Fubi@@YAPAUTrackingData@FubiUser@@I@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern IntPtr getCurrentTrackingData(UInt32 userId);

	    /**
	     * \brief Get the last tracking info of the user (one frame before the current one)
	     * (including all joint positions and orientations, the center of mass and a timestamp)
	     *
	     * @param userID OpenNI id of the user
	     * @return the user tracking info struct
	     */
        [DllImport(DLL_NAME, EntryPoint = "?getLastTrackingData@Fubi@@YAPAUTrackingData@FubiUser@@I@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern IntPtr getLastTrackingData(UInt32 userId);

        /**
         * \brief  Get the skeleton joint position out of the tracking info
         *
         * @param trackingData the trackingData struct to extract the info from
         * @param jointId
         * @param x, y, z the position of the joint
         * @param confidence the confidence for this position
         * @param timestamp (seconds since program start)
         * @param localPosition if set to true, the function will return local position (vector from parent joint)
         */
        [DllImport(DLL_NAME, EntryPoint = "?getSkeletonJointPosition@Fubi@@YAXPAUTrackingData@FubiUser@@W4Joint@SkeletonJoint@1@AAM222AAN_N@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern void getSkeletonJointPosition(IntPtr trackingData, FubiUtils.SkeletonJoint joint, out float x, out float y, out float z,
            out float confidence, out double timeStamp, [MarshalAs(UnmanagedType.U1)] bool useLocalPositions = false);

        /**
         * \brief  Get the skeleton joint orientation out of the tracking info
         *
         * @param trackingData the trackingData struct to extract the info from
         * @param jointId
         * @param mat [out] rotation 3x3 matrix (9 floats)
         * @param confidence [out] the confidence for this position
         * @param timestamp [out] (seconds since program start)
         * @param localOrientation if set to true, the function will local orientations (cleared of parent orientation) instead of globals
         */
        [DllImport(DLL_NAME, EntryPoint = "?getSkeletonJointOrientation@Fubi@@YAXPAUTrackingData@FubiUser@@W4Joint@SkeletonJoint@1@PAMMAAN_N@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern void getSkeletonJointOrientation(IntPtr trackingData, FubiUtils.SkeletonJoint joint, [MarshalAs(UnmanagedType.LPArray)] float[] mat, 
            out float confidence, out double timeStamp, [MarshalAs(UnmanagedType.U1)] bool useLocalOrientations = true);

        /**
	     * \brief  Creates an empty vector of UsertrackingData structs
	     *
	     */
        [DllImport(DLL_NAME, EntryPoint = "?createTrackingDataVector@Fubi@@YAPAV?$vector@UTrackingData@FubiUser@@V?$allocator@UTrackingData@FubiUser@@@std@@@std@@XZ"), SuppressUnmanagedCodeSecurity]
        internal static extern IntPtr createTrackingDataVector();
	
	    /**
	     * \brief  Releases the formerly created vector
	     *
	     * @param vec the vector that will be released
	     */
        [DllImport(DLL_NAME, EntryPoint = "?releaseTrackingDataVector@Fubi@@YAXPAV?$vector@UTrackingData@FubiUser@@V?$allocator@UTrackingData@FubiUser@@@std@@@std@@@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern void releaseTrackingDataVector(IntPtr vec);

	    /**
	     * \brief  Returns the size of the vector
	     *
	     * @param vec the vector that we get the size of
	     */
	    [DllImport(DLL_NAME, EntryPoint = "?getTrackingDataVectorSize@Fubi@@YAIPAV?$vector@UTrackingData@FubiUser@@V?$allocator@UTrackingData@FubiUser@@@std@@@std@@@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern UInt32 getTrackingDataVectorSize(IntPtr vec);

	    /**
	     * \brief  Returns one element of the tracking info vector
	     *
	     * @param vec the vector that we get the element of
	     */
        [DllImport(DLL_NAME, EntryPoint = "?getTrackingData@Fubi@@YAPAUTrackingData@FubiUser@@PAV?$vector@UTrackingData@FubiUser@@V?$allocator@UTrackingData@FubiUser@@@std@@@std@@I@Z"), SuppressUnmanagedCodeSecurity]
        internal static extern IntPtr getTrackingData(IntPtr vec, UInt32 index);

        /**
	     * \brief Returns the OpenNI id of the users standing closest to the sensor
	     */
        [DllImport(DLL_NAME, EntryPoint = "?getClosestUserID@Fubi@@YAIXZ"), SuppressUnmanagedCodeSecurity]
        internal static extern UInt32 getClosestUserID();

        /**
	     * \brief Stops and removes all user defined recognizers
	     */
        [DllImport(DLL_NAME, EntryPoint = "?clearUserDefinedRecognizers@Fubi@@YAXXZ"), SuppressUnmanagedCodeSecurity]
        internal static extern void clearUserDefinedRecognizers();

        /**
	     * \brief Returns all current users with their tracking information
	     * 
	     * @param pUserContainer (=0) pointer where a pointer to the current users will be stored at
	     *        The maximal size is Fubi::MaxUsers, but the current size can be requested by leaving the Pointer at 0
	     * @return the current number of users (= valid users in the container)
	     */
        [DllImport(DLL_NAME, EntryPoint = "?getCurrentUsers@Fubi@@YAGPAPAPAVFubiUser@@@Z"), SuppressUnmanagedCodeSecurity]
	    internal static extern UInt16 getCurrentUsers(IntPtr container);

        /**
	     * \brief Calculate from real world coordinates (milimeters) to screen coordinates (pixels in the depth/rgb/ir image).
	     * Uses the data of a present sensor, or alternatively calculates the projection according to given sensor values, or to standard values
	     *
	     * @param realWorldX, Y, Z vector with real world coordinates (in milimeters)
	     * @param screenX, Y, Z vector with screen coordinates (pixels in the depth/rgb/ir image)
	     * @param xRes x resolution of the screen (depth/rgb/ir image)
	     * @param yRes y resolution of the screen (depth/rgb/ir image)
	     * @param hFOV the sensors horizontal field of view 
	     * @param vFOV the sensors vertical field of view 
	     */
        [DllImport(DLL_NAME, EntryPoint = "?realWorldToProjective@Fubi@@YAXMMMAAM00HHNN@Z"), SuppressUnmanagedCodeSecurity]
	    internal static extern void realWorldToProjective(float realWorldX, float realWorldY, float realWorldZ, out float screenX, out float screenY, out float screenZ,
		int xRes = 640, int yRes = 480,	double hFOV = 1.0144686707507438, double vFOV = 0.78980943449644714);

        /**
	     * \brief resests the tracking of all users
	     */
        [DllImport(DLL_NAME, EntryPoint = "?resetTracking@Fubi@@YAXXZ"), SuppressUnmanagedCodeSecurity]
        internal static extern void resetTracking();

        /**
	     * \brief get time since program start in seconds
	     */
        [DllImport(DLL_NAME, EntryPoint = "?getCurrentTime@Fubi@@YANXZ"), SuppressUnmanagedCodeSecurity]
	    internal static extern double getCurrentTime();
    };
}
