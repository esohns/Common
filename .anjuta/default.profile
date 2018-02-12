<?xml version="1.0"?>
<anjuta>
    <plugin name="GNU Debugger" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-gdb:GdbPlugin"/>
    </plugin>
    <plugin name="Patch Plugin" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-patch:PatchPlugin"/>
    </plugin>
    <plugin name="Tools" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-tools:ATPPlugin"/>
    </plugin>
    <plugin name="API Help" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-devhelp:AnjutaDevhelp"/>
    </plugin>
    <plugin name="Quick Open" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-quick-open:QuickOpenPlugin"/>
    </plugin>
    <plugin name="Git" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-git:Git"/>
    </plugin>
</anjuta>
