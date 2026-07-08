# Solar Project 200W - Bring-Up Checklist

Last updated: 2026-07-06
Purpose: staged, current-limited bring-up baseline for safe early validation.
Primary requirement links: REQ-SAF-006, REQ-VAL-001.

## 1. Preconditions

- [ ] Requirements and architecture docs are reviewed for current session.
- [ ] Deferred decisions impacting test safety are reviewed in decisions log.
- [ ] Battery/BMS current limits are known, or conservative temporary limits are defined.
- [ ] Test instrumentation availability is confirmed (power supply, DMM, scope, current measurement).

## 2. Safety Setup

- [ ] Main battery fuse installed near battery positive terminal (REQ-SAF-001).
- [ ] Branch protection present for active test path (REQ-SAF-002).
- [ ] Current limit configured on bench source before first energization.
- [ ] Emergency shutdown path verified and documented.
- [ ] AC-domain work controls reviewed before any inverter testing (REQ-SAF-005).

## 3. Staged Bring-Up Sequence (REQ-SAF-006)

### Stage A - No-Power Verification

- [ ] Visual inspection complete (polarity, connectors, insulation, shorts).
- [ ] Continuity checks complete on critical rails.
- [ ] Protection paths identified and labeled in notes.
- [ ] Stop if any mismatch is found.

### Stage B - Low-Current Energization

- [ ] Start at conservative current limit (for example, <= 1A).
- [ ] Record input voltage/current and key node voltages.
- [ ] Confirm no abnormal thermal rise at this stage.
- [ ] Stop and investigate if instability, overcurrent, or unexpected heating occurs.

### Stage C - Incremental Current Steps

- [ ] Increase current in controlled steps only after Stage B passes.
- [ ] At each step, log voltage, current, thermal observations, and control response.
- [ ] Verify protection behavior remains intact as load increases.
- [ ] Define explicit max step for current session before test start.

### Stage D - Targeted Functional Checks

- [ ] Verify expected control and telemetry signals are present.
- [ ] Verify serial logging output for baseline observability (REQ-TEL-001).
- [ ] Confirm pass/fail outcomes for today’s selected requirements.

## 4. Verification Record (REQ-VAL-001)

| Requirement ID | Test Performed | Result (Pass/Fail) | Evidence Reference | Notes |
|---|---|---|---|---|
| REQ-SAF-006 | Staged current-limited bring-up sequence executed |  |  |  |
| REQ-VAL-001 | Requirement-to-test mapping for executed checks completed |  |  |  |

## 5. Session Closeout

- [ ] Capture unresolved issues and add/update decision log entries.
- [ ] Record configuration used (limits, instruments, firmware/hardware state).
- [ ] Record next-session first action.
- [ ] Mark this checklist revision date if process changes were made.
