/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * This is the 'C' ABI for Oboe.
 */
#ifndef OBOE_OBOEAUDIO_H
#define OBOE_OBOEAUDIO_H

#include "OboeDefinitions.h"

typedef int32_t OboeDeviceId;
typedef oboe_handle_t OboeStream;
typedef oboe_handle_t OboeStreamBuilder;
typedef oboe_handle_t OboeThread;

#define OBOE_STREAM_NONE         ((OboeStream)OBOE_HANDLE_INVALID)
#define OBOE_STREAM_BUILDER_NONE ((OboeStreamBuilder)OBOE_HANDLE_INVALID)


// ============================================================
// Audio System
// ============================================================

/**
 * @return time in the same clock domain as the timestamps
 */
oboe_nanoseconds_t Oboe_getNanoseconds(oboe_clockid_t clockid);

/**
 * The text is the ASCII symbol corresponding to the returnCode,
 * or an English message saying the returnCode is unrecognized.
 * This is intended for developers to use when debugging.
 * It is not for display to users.
 *
 * @return pointer to a text representation of an Oboe result code.
 */
const char * Oboe_convertResultToText(oboe_result_t returnCode);

/**
 * The text is the ASCII symbol corresponding to the state,
 * or an English message saying the state is unrecognized.
 * This is intended for developers to use when debugging.
 * It is not for display to users.
 *
 * @return pointer to a text representation of an Oboe state.
 */
const char * Oboe_convertStateToText(oboe_state_t state);

// ============================================================
// StreamBuilder
// ============================================================
/**
 * Create a StreamBuilder that can be used to open a Stream.
 *
 * OboeStreamBuilder_delete() must be called when you are done using the builder.
 */
oboe_result_t Oboe_createStreamBuilder(OboeStreamBuilder *builder);

/**
 * Request an audio device identified device using an ID.
 * The ID is platform specific.
 * On Android, for example, the ID could be obtained from the Java AudioManager.
 *
 * By default, the primary output device will be used.
 *
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStreamBuilder_setDeviceId(OboeStreamBuilder builder, OboeDeviceId deviceId);

/**
 * Request a sample rate in Hz.
 * The stream may be opened with a different sample rate.
 * So the application should query for the actual rate after the stream is opened.
 *
 * Technically, this should be called the "frame rate" or "frames per second",
 * because it refers to the number of complete frames transferred per second.
 * But it is traditionally called "sample rate". Se we use that term.
 *
 * Default is OBOE_UNSPECIFIED.
 *
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStreamBuilder_setSampleRate(OboeStreamBuilder builder,
                                              oboe_sample_rate_t sampleRate);

/**
 * Returns sample rate in Hertz (samples per second).
 * @return positive sampleRate or a negative error, zero for unspecified
 */
oboe_result_t OboeStreamBuilder_getSampleRate(OboeStreamBuilder builder,
                                              oboe_sample_rate_t *sampleRate);


/**
 * Request a number of samples per frame.
 * The stream may be opened with a different value.
 * So the application should query for the actual value after the stream is opened.
 *
 * Default is OBOE_UNSPECIFIED.
 *
 * Note, this quantity is sometimes referred to as "channel count".
 *
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStreamBuilder_setSamplesPerFrame(OboeStreamBuilder builder,
                                                   int32_t samplesPerFrame);

/**
 * Note, this quantity is sometimes referred to as "channel count".
 *
 * @param builder handle provided by Oboe_createStreamBuilder()
 * @param samplesPerFrame pointer to a variable to be set to samplesPerFrame.
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStreamBuilder_getSamplesPerFrame(OboeStreamBuilder builder,
                                                   int32_t *samplesPerFrame);


/**
 * Request a sample data format, for example OBOE_AUDIO_FORMAT_PCM16.
 * The application should query for the actual format after the stream is opened.
 *
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStreamBuilder_setFormat(OboeStreamBuilder builder, oboe_audio_format_t format);

/**
 * @return positive data format or a negative error, zero for unspecified
 */
oboe_result_t OboeStreamBuilder_getFormat(OboeStreamBuilder builder, oboe_audio_format_t *format);

/**
 * Request a mode for sharing the device.
 * The requested sharing mode may not be available.
 * So the application should query for the actual mode after the stream is opened.
 *
 * @param builder handle provided by Oboe_createStreamBuilder()
 * @param sharingMode OBOE_SHARING_MODE_LEGACY or OBOE_SHARING_MODE_EXCLUSIVE
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStreamBuilder_setSharingMode(OboeStreamBuilder builder,
                                               oboe_sharing_mode_t sharingMode);

/**
 * Return requested sharing mode.
 * @return OBOE_OK or a negative error
 */
oboe_result_t OboeStreamBuilder_getSharingMode(OboeStreamBuilder builder,
                                               oboe_sharing_mode_t *sharingMode);

/**
 * Request the direction for a stream. The default is OBOE_DIRECTION_OUTPUT.
 *
 * @param builder handle provided by Oboe_createStreamBuilder()
 * @param direction OBOE_DIRECTION_OUTPUT or OBOE_DIRECTION_INPUT
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStreamBuilder_setDirection(OboeStreamBuilder builder,
                                             oboe_direction_t direction);

/**
 * @param builder handle provided by Oboe_createStreamBuilder()
 * @param direction pointer to a variable to be set to the currently requested direction.
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStreamBuilder_getDirection(OboeStreamBuilder builder,
                                             oboe_direction_t *direction);

/**
 * Open a stream based on the options in the StreamBuilder.
 *
 * OboeStream_close must be called when finished with the stream to recover
 * the memory and to free the associated resources.
 *
 * @return OBOE_OK or a negative error. Also pass back a stream handle.
 */
oboe_result_t  OboeStreamBuilder_openStream(OboeStreamBuilder builder, OboeStream *stream);

/**
 * Delete the resources associated with the StreamBuilder.
 * @param builder handle provided by Oboe_createStreamBuilder()
 * @return OBOE_OK or a negative error.
 */
oboe_result_t  OboeStreamBuilder_delete(OboeStreamBuilder builder);

// ============================================================
// Stream Control
// ============================================================

/**
 * Free the resources associated with a stream created by OboeStreamBuilder_openStream()
 */
oboe_result_t  OboeStream_close(OboeStream stream);

/**
 * Asynchronously request to start playing the stream. For output streams, one should
 * write to the stream to fill the buffer before starting.
 * Otherwise it will underflow.
 * After this call the state will be in OBOE_STATE_STARTING or OBOE_STATE_STARTED.
 */
oboe_result_t  OboeStream_requestStart(OboeStream stream);

/**
 * Asynchronous request for the stream to pause.
 * Pausing a stream will freeze the data flow but not flush any buffers.
 * Use OboeStream_Start() to resume playback after a pause.
 * After this call the state will be in OBOE_STATE_PAUSING or OBOE_STATE_PAUSED.
 */
oboe_result_t  OboeStream_requestPause(OboeStream stream);

/**
 * Asynchronous request for the stream to flush.
 * Flushing will discard any pending data.
 * This call only works if the stream is pausing or paused. TODO review
 * Frame counters are not reset by a flush. They may be advanced.
 * After this call the state will be in OBOE_STATE_FLUSHING or OBOE_STATE_FLUSHED.
 */
oboe_result_t  OboeStream_requestFlush(OboeStream stream);

/**
 * Asynchronous request for the stream to stop.
 * The stream will stop after all of the data currently buffered has been played.
 * After this call the state will be in OBOE_STATE_STOPPING or OBOE_STATE_STOPPED.
 */
oboe_result_t  OboeStream_requestStop(OboeStream stream);

/**
 * Query the current state, eg. OBOE_STATE_PAUSING
 *
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStream_getState(OboeStream stream, oboe_state_t *state);

/**
 * Wait until the current state no longer matches the input state.
 *
 * <pre><code>
 * oboe_state_t currentState = OboeStream_getState(stream);
 * while (currentState >= 0 && currentState != OBOE_STATE_PAUSING) {
 *     currentState = OboeStream_waitForStateChange(
 *                                   stream, currentState, MY_TIMEOUT_MSEC);
 * }
 * </code></pre>
 *
 * @param stream A handle provided by OboeStreamBuilder_openStream()
 * @param inputState The state we want to avoid.
 * @param nextState Pointer to a variable that will be set to the new state.
 * @param timeoutNanoseconds Maximum number of nanoseconds to wait for completion.
 * @return oboe_state_t or a negative error.
 */
oboe_result_t OboeStream_waitForStateChange(OboeStream stream,
                                            oboe_state_t inputState,
                                            oboe_state_t *nextState,
                                            oboe_nanoseconds_t timeoutNanoseconds);

// ============================================================
// Stream I/O
// ============================================================

/**
 * Read data from the stream.
 *
 * The call will wait until the read is complete or until it runs out of time.
 * If timeoutNanos is zero then this call will not wait.
 *
 * Note that timeoutNanoseconds is a relative duration in wall clock time.
 * Time will not stop if the thread is asleep.
 * So it will be implemented using CLOCK_BOOTTIME.
 *
 * This call is "strong non-blocking" unless it has to wait for data.
 *
 * @param stream A stream created using OboeStreamBuilder_openStream().
 * @param buffer The address of the first sample.
 * @param numFrames Number of frames to read. Only complete frames will be written.
 * @param timeoutNanoseconds Maximum number of nanoseconds to wait for completion.
 * @return The number of frames actually written or a negative error.
 */
oboe_result_t OboeStream_read(OboeStream stream,
                               void *buffer,
                               int32_t numFrames,
                               oboe_nanoseconds_t timeoutNanoseconds);

/**
 * Write data to the stream.
 *
 * The call will wait until the write is complete or until it runs out of time.
 * If timeoutNanos is zero then this call will not wait.
 *
 * Note that timeoutNanoseconds is a relative duration in wall clock time.
 * Time will not stop if the thread is asleep.
 * So it will be implemented using CLOCK_BOOTTIME.
 *
 * This call is "strong non-blocking" unless it has to wait for room in the buffer.
 *
 * @param stream A stream created using OboeStreamBuilder_openStream().
 * @param buffer The address of the first sample.
 * @param numFrames Number of frames to write. Only complete frames will be written.
 * @param timeoutNanoseconds Maximum number of nanoseconds to wait for completion.
 * @return The number of frames actually written or a negative error.
 */
oboe_result_t OboeStream_write(OboeStream stream,
                               const void *buffer,
                               int32_t numFrames,
                               oboe_nanoseconds_t timeoutNanoseconds);


// ============================================================
// High priority audio threads
// ============================================================

/**
 * Create a thread with special properties for low latency audio performance.
 * This thread can be used to implement a callback API.
 *
 * Note that this API is in flux.
 *
 * @param threadHandlePtr a pointer to receive a thread handle
 * @param periodNanoseconds the estimated period at which the audio thread will need to wake up
 * @param start_routine your thread entry point
 * @param arg an argument that will be passed to your thread entry point
 * @return OBOE_OK or a negative error.
 */
oboe_result_t Oboe_createAudioThread(OboeThread *threadHandlePtr,
                                     oboe_nanoseconds_t periodNanoseconds,
                                     void *(*start_routine)(void *), void *arg);

/**
 * Wait until the thread exits or an error occurs.
 * The thread handle will be deleted.
 *
 * @param thread the thread handle passed back from Oboe_createAudioThread()
 * @param returnArg a pointer to a variable to receive the return value
 * @param timeoutNanoseconds Maximum number of nanoseconds to wait for completion.
 * @return OBOE_OK or a negative error.
 */
oboe_result_t Oboe_joinAudioThread(OboeThread thread,
                                   void **returnArg,
                                   oboe_nanoseconds_t timeoutNanoseconds);

// ============================================================
// Stream - queries
// ============================================================


/**
 * This can be used to adjust the latency of the buffer by changing
 * the threshold where blocking will occur.
 * By combining this with OboeStream_getUnderrunCount(), the latency can be tuned
 * at run-time for each device.
 *
 * This cannot be set higher than OboeStream_getBufferCapacity().
 *
 * @param stream handle provided by OboeStreamBuilder_openStream()
 * @param frames requested number of frames that can be filled without blocking
 * @return actual number of frames or a negative error
 */
oboe_result_t OboeStream_setBufferSize(OboeStream stream, oboe_size_frames_t frames);

/**
 * Query the maximum number of frames that can be filled without blocking.
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStream_getBufferSize(OboeStream stream, oboe_size_frames_t *frames);

/**
 * Query the number of frames that are read or written by the endpoint at one time.
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStream_getFramesPerBurst(OboeStream stream, oboe_size_frames_t *frames);

/**
 * Query maximum buffer capacity in frames.
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStream_getBufferCapacity(OboeStream stream, oboe_size_frames_t *frames);

/**
 * An XRun is an Underrun or an Overrun.
 * During playing, an underrun will occur if the stream is not written in time
 * and the system runs out of valid data.
 * During recording, an overrun will occur if the stream is not read in time
 * and there is no place to put the incoming data so it is discarded.
 *
 * An underrun or overrun can cause an audible "pop" or "glitch".
 *
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStream_getXRunCount(OboeStream stream, int32_t *xRunCount);

/**
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStream_getSampleRate(OboeStream stream, int32_t *sampleRate);

/**
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStream_getSamplesPerFrame(OboeStream stream, int32_t *samplesPerFrame);

/**
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStream_getFormat(OboeStream stream, oboe_audio_format_t *format);

/**
 * Provide actual sharing mode.
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStream_getSharingMode(OboeStream stream,
                                        oboe_sharing_mode_t *sharingMode);

/**
 * @param stream handle provided by OboeStreamBuilder_openStream()
 * @param direction pointer to a variable to be set to the current direction.
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStream_getDirection(OboeStream stream, uint32_t *direction);

/**
 * Passes back the number of frames that have been written since the stream was created.
 * For an output stream, this will be advanced by the application calling write().
 * For an input stream, this will be advanced by the device or service.
 *
 * The frame position is monotonically increasing.
 *
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStream_getFramesWritten(OboeStream stream, oboe_position_frames_t *frames);

/**
 * Passes back the number of frames that have been read since the stream was created.
 * For an output stream, this will be advanced by the device or service.
 * For an input stream, this will be advanced by the application calling read().
 *
 * The frame position is monotonically increasing.
 *
 * @return OBOE_OK or a negative error.
 */
oboe_result_t OboeStream_getFramesRead(OboeStream stream, oboe_position_frames_t *frames);

/**
 * Passes back the time at which a particular frame was presented.
 * This can be used to synchronize audio with video or MIDI.
 * It can also be used to align a recorded stream with a playback stream.
 *
 * Timestamps are only valid when the stream is in OBOE_STATE_STARTED.
 * OBOE_ERROR_INVALID_STATE will be returned if the stream is not started.
 * Note that because requestStart() is asynchronous, timestamps will not be valid until
 * a short time after calling requestStart().
 * So OBOE_ERROR_INVALID_STATE should not be considered a fatal error.
 * Just try calling again later.
 *
 * If an error occurs, then the position and time will not be modified.
 *
 * The position and time passed back are monotonically increasing.
 *
 * @param stream A handle provided by OboeStreamBuilder_openStream()
 * @param clockId OBOE_CLOCK_MONOTONIC or OBOE_CLOCK_BOOTTIME
 * @param framePosition pointer to a variable to receive the position
 * @param timeNanoseconds pointer to a variable to receive the time
 * @return OBOE_OK or a negative error
 */
oboe_result_t OboeStream_getTimestamp(OboeStream stream,
                                      oboe_clockid_t clockid,
                                      oboe_position_frames_t *framePosition,
                                      oboe_nanoseconds_t *timeNanoseconds);

#endif //NATIVEOBOE_OBOEAUDIO_H
